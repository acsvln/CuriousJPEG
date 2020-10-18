#include "sos_decoder.hpp"

#include <bitset>
#include <boost/assert.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <map>

#include "data_reader.hpp"
#include "exceptions.hpp"
#include "testing_utility.hpp"

namespace {

auto findComponentById(const std::vector<DCTComponent> &Components,
                       const std::size_t Id) {
  return std::find_if(
      std::begin(Components), std::end(Components),
      [Id](const auto &Component) { return Component.Id == Id; });
}

} // namespace

auto SOSDecoder::locateNodeInHuffmanTree(
    BitExtractor &Extractor, const std::shared_ptr<HuffmanTree::Node> &Tree)
    -> std::shared_ptr<HuffmanTree::Node> {
  auto Node = Tree;

  do {
    const auto BitNum = Extractor.nextNumber();

    switch (BitNum) {
    case 0:
      Node = Node->left();
      break;
    case 1:
      Node = Node->right();
      break;
    default:
      BOOST_ASSERT(false);
    }

    if (nullptr == Node) {
      throw InvalidJPEGDataException{"Can not locate correct Node in Huffman tree"};
    }

  } while (!Node->isLeaf());

  return Node;
}

auto SOSDecoder::readDU(BitExtractor &Extractor,
                        const std::shared_ptr<HuffmanTree::Node> &DC_Tree,
                        const std::shared_ptr<HuffmanTree::Node> &AC_Tree)
    -> Matrix16 {
  std::array<int16_t, 64> Buffer = {0};

  const auto extractAndNorm = [&](const std::size_t BitCount) -> int16_t {
    const auto RawNumber = static_cast<int16_t>(Extractor.nextNumber(BitCount));
    std::bitset<16> set{static_cast<unsigned long long>(RawNumber)};
    if (0 != set[BitCount - 1]) {
      return RawNumber;
    }
    return RawNumber - static_cast<int16_t>(std::pow(2, BitCount)) + 1;
  };

  const auto DC_Node = locateNodeInHuffmanTree(Extractor, DC_Tree);
  const auto DC_Value = DC_Node->data();
  if (0 == DC_Value) {
    Buffer[0] = 0;
  } else {
    Buffer[0] = extractAndNorm(DC_Value);
  }

  auto Iterator = std::next(Buffer.begin());
  do {
    const auto AC_Node = locateNodeInHuffmanTree(Extractor, AC_Tree);
    const auto AC_Value = AC_Node->data();
    if (0 == AC_Value) {
      // матрица уже заполнена нулями
      return CreateZigZagMatrix(Buffer);
    }

    const auto Left = std::distance(Iterator, Buffer.end());

    // добавляем нули
    const auto NullCount = highByte(AC_Value);
    if (NullCount > Left) {
        throw InvalidJPEGDataException{"Wrong count of null's in data unit"};
    }

    Iterator += NullCount;

    // добавляем значение
    if (const auto ValueLength = lowByte(AC_Value); 0 != ValueLength) {
      if (Buffer.end() == Iterator) {
        throw InvalidJPEGDataException{"AC table in source file is incorrectly large"};
      }

      *Iterator = extractAndNorm(ValueLength);
      ++Iterator;
    }
  } while (Buffer.end() != Iterator);

  return CreateZigZagMatrix(Buffer);
}

auto SOSDecoder::readMCU(
    BitExtractor &Extractor, const DCTTable &DCT,
    const std::vector<Channel> &Channels,
    const std::vector<std::shared_ptr<HuffmanTree::Node>> &AC_Tables,
    const std::vector<std::shared_ptr<HuffmanTree::Node>> &DC_Tables)
    -> MinimumCodedUnit {
  MinimumCodedUnit Result;

  const auto extractDataUnitForChannel =
      [&](const Channel &Chann) -> std::vector<DataUnit> & {
    switch (Chann.Id) {
    case 1:
      return Result.Cs1;
    case 2:
      return Result.Cs2;
    case 3:
      return Result.Cs3;
    }
    throw InvalidJPEGDataException{"Wrong channel"};
  };

  for (const auto &Channel : Channels) {
    const auto Component = findComponentById(DCT.Components, Channel.Id);

    if (std::end(DCT.Components) == Component) {
      throw InvalidJPEGDataException{"Wrong channel"};
    }

    const auto AC_Root = AC_Tables.at(Channel.AC_Id);
    const auto DC_Root = DC_Tables.at(Channel.DC_Id);

    auto &Data = extractDataUnitForChannel(Channel);

    const std::size_t DataUnitCount = Component->H * Component->V;
    Data.reserve(DataUnitCount);
    for (std::size_t t = 0; t < DataUnitCount; t++) {
      std::back_inserter(Data) = readDU(Extractor, DC_Root, AC_Root);
    }

    if (Data.size() > 1) {
      for (auto Next = Data.begin() + 1, Current = Data.begin();
           Current < Data.end() && Next < Data.end(); ++Current, ++Next) {
        auto &NextMatrix = *Next;
        const auto &Matrix = *Current;
        NextMatrix(0, 0) = Matrix(0, 0) + NextMatrix(0, 0);
      }
    }
  }

  return Result;
}

// TODO: проверить размерность матриц в QuantVector'e
auto SOSDecoder::quantMCU(MinimumCodedUnit &&MCU,
                          const std::vector<DCTComponent> &Components,
                          const std::vector<Matrix16u> &QuantVector)
    -> MinimumCodedUnit {
  const auto quantChannel = [&](auto &ChannelData,
                                const std::size_t ChannelId) {
    const auto Component = findComponentById(Components, ChannelId);
    if (Component == std::end(Components)) {
      throw InvalidJPEGDataException{"Wrong channel"};
    }
    for (auto &Matrix : ChannelData) {
      const auto DQT = QuantVector.at(Component->DQT_Id);
      // домножили на матрицу квантования
      Matrix = boost::numeric::ublas::element_prod(Matrix, DQT);
    }
  };

  quantChannel(MCU.Cs1, 1);
  quantChannel(MCU.Cs2, 2);
  quantChannel(MCU.Cs3, 3);

  return std::move(MCU);
}

auto SOSDecoder::reverseDQT(const Matrix16 &Matrix) -> Matrix16 {
  const auto Cx = [](const auto X) -> double {
    return (0 == X) ? (1. / std::sqrt(2.)) : 1.;
  };

  Matrix16 Result(Matrix.size1(), Matrix.size2());
  const auto pi = boost::math::constants::pi<double>();

  for (std::size_t Y = 0; Y < Matrix.size1(); Y++) {
    for (std::size_t X = 0; X < Matrix.size2(); X++) {
      double Accumulator = 0.;
      for (std::size_t U = 0; U < 8; U++) {
        for (std::size_t V = 0; V < 8; V++) {
          const double Cu = Cx(U);
          const double Cv = Cx(V);
          const double Svu = Matrix(V, U); // here v u
          Accumulator += Cu * Cv * Svu * std::cos((2 * X + 1) * U * pi / 16.) *
                         std::cos((2 * Y + 1) * V * pi / 16.);
        }
      }
      Result(Y, X) = static_cast<short>(Accumulator / 4.);
    }
  }

  return Result;
}

auto SOSDecoder::normalizeReversedDQT(Matrix16 &&Table) -> Matrix16 {
  for (std::size_t i = 0; i < Table.size1(); i++) {
    for (std::size_t j = 0; j < Table.size2(); j++) {
      Table(i, j) = std::clamp<int16_t>(Table(i, j) + 128, 0, 255);
    }
  }
  return Table;
}

auto SOSDecoder::convertYCbCrToRGB(const Matrix16 &Y, const Matrix16 &Cb,
                                   const Matrix16 &Cr)
    -> std::tuple<Matrix8u, Matrix8u, Matrix8u> {
  const auto normalize = [](const double Value) -> uint8_t {
    return static_cast<uint8_t>(std::clamp(Value, 0., 255.));
  };

  boost::numeric::ublas::matrix<uint8_t> R(16, 16);
  boost::numeric::ublas::matrix<uint8_t> G(16, 16);
  boost::numeric::ublas::matrix<uint8_t> B(16, 16);

  for (std::size_t i = 0; i < 16; i++) {
    for (std::size_t j = 0; j < 16; j++) {
      R(i, j) =
          normalize(std::round(Y(i, j) + 1.402 * (Cr(i / 2, j / 2) - 128.)));
      G(i, j) =
          normalize(std::round(Y(i, j) - 0.34414 * (Cb(i / 2, j / 2) - 128.) -
                               0.71414 * (Cr(i / 2, j / 2) - 128.)));
      B(i, j) =
          normalize(std::round(Y(i, j) + 1.772 * (Cb(i / 2, j / 2) - 128.)));
    }
  }

  return {R, G, B};
}

SOSDecoder::SOSDecoder() : DecoderBase{"Start Of Scan"} {}

void SOSDecoder::InvokeImpl(std::istream &Stream, DecoderContext &Context) {
  const auto ChannelCount = DataReader::readNumber<uint8_t>(Stream);
  if (3 != ChannelCount) {
    throw NotImplementedException{"Start of decoding - invalid channel's count"};
  }

  std::vector<Channel> Channels;

  for (std::size_t Index = 0; Index < ChannelCount; ++Index) {
    const auto Id = DataReader::readNumber<uint8_t>(Stream);
    const auto HuffId = DataReader::readNumber<uint8_t>(Stream);
    Channel Chann;
    Chann.Id = Id;
    Chann.AC_Id = HuffId >> 4;
    Chann.DC_Id = HuffId & 0xF;
    Channels.push_back(Chann);
  }

  DataReader::skipChars(Stream, 3);

  using Matrix = Matrix16u;
  using ImageData = std::map<int, std::vector<Matrix>>;

  std::vector<ImageData> imageData;
  BitExtractor Extractor{Stream};

  do {
    auto MCU = readMCU(Extractor, Context.dct, Channels,
                       Context.AC_HuffmanTables, Context.DC_HuffmanTables);

    MCU = quantMCU(std::move(MCU), Context.dct.Components, Context.DQT_Vector);

    BOOST_ASSERT_MSG((MCU.Cs1.size() == MCU.Cs2.size()) &&
                         (MCU.Cs2.size() == MCU.Cs3.size()),
                     "Channel size is different");

    Matrix matrix(16, 16);

    for (std::size_t i = 0; i < MCU.Cs2.size(); i += 4) {
      auto Y1 = reverseDQT(MCU.Cs1.at(i));
      auto Y2 = reverseDQT(MCU.Cs1.at(i + 1));
      auto Y3 = reverseDQT(MCU.Cs1.at(i + 2));
      auto Y4 = reverseDQT(MCU.Cs1.at(i + 3));
      auto Cb = MCU.Cs2.at(i);
      auto Cr = MCU.Cs3.at(i);

      Matrix Y(16, 16);

      const auto HalfRowsCount = 8;
      const auto HalfColsCount = 8;

      for (std::size_t Row = 0; Row < HalfRowsCount; Row++) {
        for (std::size_t Col = 0; Col < HalfColsCount; Col++) {
          Y(Row, Col) = Y1(Row, Col);
          Y(Row, 8 + Col) = Y2(Row, Col);
          Y(8 + Row, Col) = Y3(Row, Col);
          Y(8 + Row, 8 + Col) = Y4(Row, Col);
        }
      }

      auto RevercedY = normalizeReversedDQT(std::move(Y));
      auto RevercedCb = normalizeReversedDQT(reverseDQT(Cb));
      auto RevercedCr = normalizeReversedDQT(reverseDQT(Cr));

      const auto [R, G, B] =
          convertYCbCrToRGB(RevercedY, RevercedCb, RevercedCr);
      Context.Image.push_back({R, G, B});
    }
  } while (false);
  std::cout << "while (false)" << std::endl;
}
