#include "sos_decoder.hpp"
#include "utility.hpp"

#include <map>
#include <bitset>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/assert.hpp>
#include <tiffio.h>
#include "data_reader.hpp"
#include "testing_utility.hpp"

auto SOSDecoder::locateNodeInHuffmanTree(
    BitExtractor &Extractor, std::shared_ptr<HuffmanTree::Node> const &Tree)
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

    if ( nullptr == Node ) {
        throw std::runtime_error{"Invalid JPEG Data"};
    }

  } while (!Node->isLeaf());

  return Node;
}

namespace {

auto locateNodeInHuffmanTree16(
    BitExtractor &Extractor, std::shared_ptr<HuffmanTree::Node> const &Tree)
    -> std::shared_ptr<HuffmanTree::Node> {
  auto Node = Tree;

  do {
    const auto BitNum = Extractor.nextNumber16();

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

    if ( nullptr == Node ) {
        throw std::runtime_error{"Invalid JPEG Data"};
    }

  } while (!Node->isLeaf());

  return Node;
}

void printVector( const std::array<int16_t, 64>& vector ) {
    std::cout << vector.size() << ':' << std::endl;
    for ( const auto& el : vector ) {
        std::cout << el << '\t';
    }
    std::cout << std::endl;
}

auto findComponentById( std::vector<DCTComponent> const& Components, std::size_t const Id ) {
    return std::find_if(
              std::begin( Components )
            , std::end( Components )
            , [Id]( const auto& Component ){
        return Component.Id == Id;
    } );
}

template<typename T>
auto
reverseDQT_Impl(boost::numeric::ublas::matrix<T> const& Matrix) ->boost::numeric::ublas::matrix<T> {
    const auto Cx = []( const auto X ) -> double {
        return ( 0 == X ) ? (1. / std::sqrt( 2. )) : 1.;
    };

    boost::numeric::ublas::matrix<T> Result(Matrix.size1(),Matrix.size2());
    const auto pi = boost::math::constants::pi<double>();

    for ( std::size_t Y = 0; Y < Matrix.size1(); Y++ ) {
        for ( std::size_t X = 0; X < Matrix.size2(); X++ ) {
            double Accumulator = 0.;
            for ( std::size_t U = 0; U < 8; U++ ) {
                for ( std::size_t V = 0; V < 8; V++ ) {
                    const double Cu = Cx(U);
                    const double Cv = Cx(V);
                    const double Svu = Matrix(V,U); // here v u
                    Accumulator += Cu * Cv * Svu
                        * std::cos( ( 2 * X + 1 ) * U * pi / 16. )
                        * std::cos( ( 2 * Y + 1 ) * V * pi / 16. );
                }
            }
            Result(Y,X) = static_cast<short>( Accumulator / 4. );
        }
    }

    return Result;
}

//          1     7   7                      (2x+1)*u*Pi      (2x+1)*v*Pi
//  f(x,y)= - * [Sum Sum C(u)*C(v)*F(u,v)*cos----------- * cos----------- ]
//          4    x=0 y=0                         16                16

template<typename T>
auto
reverseDQT_Impl2(boost::numeric::ublas::matrix<T> const& Matrix) ->boost::numeric::ublas::matrix<T> {
    const auto Cx = []( const auto X ) -> double {
        return ( 0 == X ) ? (1. / std::sqrt( 2. )) : 1.;
    };

    boost::numeric::ublas::matrix<T> Result(Matrix.size1(),Matrix.size2());
    const auto pi = boost::math::constants::pi<double>();

    for ( std::size_t U = 0; U < Matrix.size1(); U++ ) {
        for ( std::size_t V = 0; V < Matrix.size2(); V++ ) {
            double Accumulator = 0.;
            for ( std::size_t Y = 0; Y < 8; Y++ ) {
                for ( std::size_t X = 0; X < 8; X++ ) {
                    const double Cu = Cx(U);
                    const double Cv = Cx(V);
                    const double Svu = Matrix(U,V); // here v u
                    Accumulator += Cu * Cv * Svu
                        * std::cos( ( 2 * Y + 1 ) * V * pi / 16. )
                        * std::cos( ( 2 * X + 1 ) * U * pi / 16. );
                }
            }
            Result(U,V) = static_cast<short>( Accumulator / 4. );
        }
    }

    return Result;
}

template<typename T>
auto
reverseDQT_Impl3(boost::numeric::ublas::matrix<T> const& Matrix) ->boost::numeric::ublas::matrix<T> {
    const auto Cx = []( const auto X ) -> double {
        return ( 0 == X ) ? (1. / std::sqrt( 2. )) : 1.;
    };

    boost::numeric::ublas::matrix<T> Result(Matrix.size1(),Matrix.size2());
    const auto pi = boost::math::constants::pi<double>();

    for ( std::size_t Y = 0; Y < Matrix.size1(); Y++ ) {
        for ( std::size_t X = 0; X < Matrix.size2(); X++ ) {
            double Accumulator = 0.;
            for ( std::size_t U = 0; U < 8; U++ ) {
                for ( std::size_t V = 0; V < 8; V++ ) {
                    const double Cu = Cx(U);
                    const double Cv = Cx(V);
                    const double Svu = Matrix(V,U); // here v u
                    Accumulator += Cu * Cv * Svu
                        * std::cos( ( 2 * X + 1 ) * U * pi / 16. )
                        * std::cos( ( 2 * Y + 1 ) * V * pi / 16. );
                }
            }
            Result(Y,X) = static_cast<short>( Accumulator / 4. );
        }
    }

    return Result;
}

}

auto SOSDecoder::readDU(BitExtractor &Extractor,
                        std::shared_ptr<HuffmanTree::Node> const &DC_Tree,
                        std::shared_ptr<HuffmanTree::Node> const &AC_Tree)
    -> boost::numeric::ublas::matrix<int16_t> {
  std::array<int16_t, 64> Buffer = {0};

  const auto extractAndNorm = [&](std::size_t const BitCount) -> int16_t {
    const auto RawNumber = static_cast<int16_t>(Extractor.nextNumber16(BitCount));
    std::bitset<16> set{static_cast<unsigned long long>(RawNumber)};
    if (0 != set[BitCount - 1]) {
      return RawNumber;
    }
    return RawNumber - static_cast<int16_t>(std::pow(2, BitCount)) + 1;
  };

  const auto DC_Node = locateNodeInHuffmanTree16(Extractor, DC_Tree);
  const auto DC_Value = DC_Node->data();
  if (0 == DC_Value) {
    Buffer[0] = 0;
  } else {
    Buffer[0] = extractAndNorm(DC_Value);
  }

  auto Iterator = std::next(Buffer.begin());
  do {
    const auto AC_Node = locateNodeInHuffmanTree16(Extractor, AC_Tree);
    const auto AC_Value = AC_Node->data();
    if (0 == AC_Value) {
      // матрица уже заполнена нулями
      return CreateZigZagMatrix(Buffer);
    }

    const auto Left = std::distance(Iterator, Buffer.end());

    // добавляем нули
    const auto NullCount = lowByte(AC_Value);
    if (NullCount > Left) {
      throw std::runtime_error{"Invalid JPEG Data"};
    }

    Iterator += NullCount;

    // добавляем значение
    if (const auto ValueLength = highByte(AC_Value); 0 != ValueLength) {
      if (Buffer.end() == Iterator) {
         throw std::runtime_error{"Invalid JPEG Data"};
      }

      *Iterator = extractAndNorm(ValueLength);
      ++Iterator;
    }
  } while (Buffer.end() != Iterator);

  return CreateZigZagMatrix(Buffer);
}

auto SOSDecoder::readMCU(
    BitExtractor& Extractor,
    DCTTable const& DCT,
    std::vector<Channel> const& Channels,
    std::vector<std::shared_ptr<HuffmanTree::Node>> const& AC_Tables,
    std::vector<std::shared_ptr<HuffmanTree::Node>> const& DC_Tables ) -> MinimumCodedUnit
{
    MinimumCodedUnit Result;
    
    const auto extractDataUnitForChannel = [&]( Channel const& Chann ) -> std::vector<DataUnit>& {
        switch ( Chann.Id ) {
        case 1:
            return Result.Cs1;
        case 2:
            return Result.Cs2;
        case 3:
            return Result.Cs3;
        }
        throw std::runtime_error{"Invalid channel"};
    };

    for ( const auto& Channel : Channels ) {
        const auto Component = findComponentById(DCT.Components,Channel.Id);

        if ( std::end( DCT.Components ) == Component ) {
            throw std::runtime_error{"Invalid JPEG Data"};
        }

        const auto AC_Root = AC_Tables.at(Channel.AC_Id);
        const auto DC_Root = DC_Tables.at(Channel.DC_Id);

        auto& Data = extractDataUnitForChannel(Channel);

        std::size_t const DataUnitCount = Component->H * Component->V;
        Data.reserve( DataUnitCount );
        for ( std::size_t t = 0; t < DataUnitCount; t++ ) {
            std::back_inserter(Data) = readDU(
                Extractor,
                DC_Root,
                AC_Root
            );
        }

        if ( Data.size() > 1 ) {
            for (   auto Next = Data.begin() + 1, Current = Data.begin();
                    Current < Data.end() && Next < Data.end();
                    ++Current, ++Next ) {
                auto& NextMatrix = *Next;
                const auto& Matrix = *Current;
                NextMatrix(0,0) = Matrix(0,0) + NextMatrix(0,0);
            }
        }
    }
    
    return Result;
}

// TODO: проверить размерность матриц в QuantVector'e
auto SOSDecoder::quantMCU(
      /*SOSDecoder::*/MinimumCodedUnit&& MCU
    , std::vector<DCTComponent> const& Components
    , std::vector<boost::numeric::ublas::matrix<uint16_t>> const& QuantVector ) -> MinimumCodedUnit
{
    const auto quantChannel = [&]( auto& ChannelData, std::size_t const ChannelId ) {
        const auto Component = findComponentById(Components, ChannelId);
        if ( Component == std::end( Components ) ) {
            throw std::runtime_error{"Invalid channel"};
        }
        for ( auto& Matrix : ChannelData ) {
            const auto DQT = QuantVector.at(Component->DQT_Id);
            // домножили на матрицу квантования
            Matrix = boost::numeric::ublas::element_prod(Matrix, DQT);
        }
    };

    quantChannel(MCU.Cs1, 1);
    quantChannel(MCU.Cs2, 2);
    quantChannel(MCU.Cs3, 3);

    return std::move( MCU );
}

auto
SOSDecoder::reverseDQT(boost::numeric::ublas::matrix<int16_t> const& Matrix) ->boost::numeric::ublas::matrix<int16_t> {
    return reverseDQT_Impl3( Matrix );
}

auto
SOSDecoder::reverseDQT2(boost::numeric::ublas::matrix<int16_t> const& Matrix) -> boost::numeric::ublas::matrix<int16_t> {
    //          1     7   7                      (2x+1)*u*Pi      (2x+1)*v*Pi
    //  f(x,y)= - * [Sum Sum C(u)*C(v)*F(u,v)*cos----------- * cos----------- ]
    //          4    x=0 y=0                         16                16

    const auto Cx = []( const auto X ) -> double {
        return ( 0 == X ) ? (1. / std::sqrt( 2. )) : 1.;
    };

    boost::numeric::ublas::matrix<int16_t> Result(Matrix.size1(),Matrix.size2());
    const auto pi = boost::math::constants::pi<double>();

    for ( std::size_t U = 0; U < Matrix.size1(); U++ ) {
        for ( std::size_t V = 0; V < Matrix.size2(); V++ ) {
            double Accumulator = 0.;
            for ( std::size_t X = 0; X < Matrix.size1(); X++ ) {
                for ( std::size_t Y = 0; Y < Matrix.size2(); Y++ ) {
                    const double Cu = Cx(X);
                    const double Cv = Cx(Y);
                    const double Svu = Matrix(Y,X); // here v u
                    Accumulator += Cu * Cv * Svu
                        * std::cos( ( 2 * V + 1 ) * X * pi / 16. )
                        * std::cos( ( 2 * U + 1 ) * Y * pi / 16. );
                }
            }
            Result(U,V) = static_cast<short>( Accumulator / 4. );
        }
    }

    return Result;
}

auto
SOSDecoder::normalizeReversedDQT(boost::numeric::ublas::matrix<int16_t>&& Table) -> boost::numeric::ublas::matrix<int16_t> {
    for (std::size_t i = 0; i < Table.size1(); i++) {
      for (std::size_t j = 0; j < Table.size2(); j++) {
        Table(i, j) = std::clamp<int16_t>( Table(i, j) + 128, 0, 255 );
      }
    }
    return Table;
}

//-------------------------------------

//ivect4 ycrcb2rgb(int y, int cr, int cb)
//{
//    int r = round(1.402*(cr-128) + y);
//    int g = round(-0.34414*(cb-128)-0.71414*(cr-128) + y);
//    int b = round(1.772*(cb-128) + y);
//    return ivect4(r, g, b, 255);
//}

// https://impulseadventure.com/photo/jpeg-color-space.html
//  R = round(Y                      + 1.402   * (Cr-128))
//  G = round(Y - 0.34414 * (Cb-128) - 0.71414 * (Cr-128))
//  B = round(Y + 1.772   * (Cb-128)                     )
auto
SOSDecoder::convertYCbCrToRGB(
      boost::numeric::ublas::matrix<int16_t> const& Y
    , boost::numeric::ublas::matrix<int16_t> const& Cb
    , boost::numeric::ublas::matrix<int16_t> const& Cr ) -> std::tuple<
      boost::numeric::ublas::matrix<int16_t>
    , boost::numeric::ublas::matrix<int16_t>
    , boost::numeric::ublas::matrix<int16_t>
>
{
    const auto normalize = []( double const Value ) -> int16_t {
        return static_cast<int16_t>( std::clamp( Value, 0., 255. ) );
    };

    boost::numeric::ublas::matrix<int16_t> R(8,8);
    boost::numeric::ublas::matrix<int16_t> G(8,8);
    boost::numeric::ublas::matrix<int16_t> B(8,8);

    for ( std::size_t i = 0; i < 8; i++ ) {
        for ( std::size_t j = 0; j < 8; j++ ) {
            R(i,j) = normalize( std::round( Y(i,j)                               + 1.402   * ( Cr(i,j) - 128. ) ) );
            G(i,j) = normalize( std::round( Y(i,j) - 0.34414 * ( Cb(i,j) - 128. ) - 0.71414 * ( Cr(i,j) - 128. ) ) );
            B(i,j) = normalize( std::round( Y(i,j) + 1.772   * ( Cb(i,j) - 128. ) ) );
        }
    }

    return {R, G, B};
}

auto
SOSDecoder::convertYCbCrToRGB_AL(boost::numeric::ublas::matrix<int16_t> const &Y,
           boost::numeric::ublas::matrix<int16_t> const &Cb,
           boost::numeric::ublas::matrix<int16_t> const &Cr) ->
  std::tuple<
      boost::numeric::ublas::matrix<uint8_t>,
      boost::numeric::ublas::matrix<uint8_t>,
      boost::numeric::ublas::matrix<uint8_t>
> {
    const auto normalize = []( double const Value ) -> uint8_t {
        return static_cast<uint8_t>( std::clamp( Value, 0., 255. ) );
    };

    boost::numeric::ublas::matrix<uint8_t> R(16,16);
    boost::numeric::ublas::matrix<uint8_t> G(16,16);
    boost::numeric::ublas::matrix<uint8_t> B(16,16);

    for ( std::size_t i = 0; i < 16; i++ ) {
        for ( std::size_t j = 0; j < 16; j++ ) {
            R(i,j) = normalize( std::round( Y(i,j)                               + 1.402   * ( Cr(i/2,j/2) - 128. )     ) );
            G(i,j) = normalize( std::round( Y(i,j) - 0.34414 * ( Cb(i/2,j/2) - 128. ) - 0.71414 * ( Cr(i/2, j/2) - 128. ) ) );
            B(i,j) = normalize( std::round( Y(i,j) + 1.772   * ( Cb(i/2,j/2) - 128. )                                   ) );
        }
    }

    return {R, G, B};
}

SOSDecoder::SOSDecoder() : Decoder{"Start Of Scan"} {}

void SOSDecoder::InvokeImpl(std::istream &Stream, Context &Ctx) {
    const auto ChannelCount = DataReader::readNumber<uint8_t>(Stream);
    if ( 3 != ChannelCount ) {
        throw std::runtime_error{"Invalid channel's count"};
    }

    std::vector<Channel> Channels;

    for ( std::size_t Index = 0; Index < ChannelCount; ++Index ) {
        const auto Id = DataReader::readNumber<uint8_t>(Stream);
        const auto HuffId = DataReader::readNumber<uint8_t>(Stream);
        Channel Chann;
        Chann.Id = Id;
        Chann.AC_Id = HuffId >> 4;
        Chann.DC_Id = HuffId & 0xF;
        Channels.push_back( Chann );
    }

    DataReader::skipChars(Stream,3);

    using Matrix = boost::numeric::ublas::matrix<uint16_t>;
    using ImageData = std::map<int, std::vector<Matrix>>;

    std::vector<ImageData> imageData;
    BitExtractor Extractor{ Stream };

    do {
        auto MCU = readMCU(
            Extractor,
            Ctx.dct,
            Channels,
            Ctx.AC_HuffmanTables,
            Ctx.DC_HuffmanTables
        );

        MCU = quantMCU( std::move( MCU ), Ctx.dct.Components, Ctx.DQT_Vector );

        BOOST_ASSERT_MSG( ( MCU.Cs1.size() == MCU.Cs2.size() ) && ( MCU.Cs2.size() == MCU.Cs3.size() ),
                         "Channel size is different" );

        Matrix matrix( 16, 16 );
////////////////////////////////////////////

//        const auto R1 = TestedDecoder::reverseDQT(Y1_Raw);
//        const auto R2 = TestedDecoder::reverseDQT(Y2_Raw);
//        const auto R3 = TestedDecoder::reverseDQT(Y3_Raw);
//        const auto R4 = TestedDecoder::reverseDQT(Y4_Raw);

//        printMatrix(R1);
//        printMatrix(R2);
//        printMatrix(R3);
//        printMatrix(R4);

//        for ( std::size_t Row = 0; Row < HalfRowsCount; Row++ ) {
//            for ( std::size_t Col = 0; Col < HalfColsCount; Col++ ) {
//               Y_Raw(     Row,     Col ) = R1(Row,Col);
//               Y_Raw(     Row, 8 + Col ) = R2(Row,Col);
//               Y_Raw( 8 + Row,     Col ) = R3(Row,Col);
//               Y_Raw( 8 + Row, 8 + Col ) = R4(Row,Col);
//            }
//        }
////////////////////////////////////////////////
        for ( std::size_t i = 0; i < MCU.Cs2.size(); i += 4 ) {
            auto Y1 = reverseDQT( MCU.Cs1.at(i) );
            auto Y2 = reverseDQT( MCU.Cs1.at(i + 1) );
            auto Y3 = reverseDQT( MCU.Cs1.at(i + 2) );
            auto Y4 = reverseDQT( MCU.Cs1.at(i + 3) );
            auto Cb = MCU.Cs2.at(i);
            auto Cr = MCU.Cs3.at(i);

            Matrix Y( 16, 16 );

            const auto HalfRowsCount = 8;
            const auto HalfColsCount = 8;

            for ( std::size_t Row = 0; Row < HalfRowsCount; Row++ ) {
                for ( std::size_t Col = 0; Col < HalfColsCount; Col++ ) {
                   Y(     Row,     Col ) = Y1(Row,Col);
                   Y(     Row, 8 + Col ) = Y2(Row,Col);
                   Y( 8 + Row,     Col ) = Y3(Row,Col);
                   Y( 8 + Row, 8 + Col ) = Y4(Row,Col);
                }
            }

//            for ( std::size_t i = 0; i < 8; i++ ) {
//                for ( std::size_t j = 0; j < 8; j++ ) {
//                   Y(     i,     j ) = Y1(i,j);
//                   Y(     i, 8 + j ) = Y3(i,j);
//                   Y( 8 + i,     j ) = Y2(i,j);
//                   Y( 8 + i, 8 + j ) = Y4(i,j);
//                }
//            }

            auto RevercedY = normalizeReversedDQT( std::move( Y ) );
            auto RevercedCb = normalizeReversedDQT( reverseDQT( Cb ) );
            auto RevercedCr = normalizeReversedDQT( reverseDQT( Cr ) );

            printMatrix(RevercedY);
            printMatrix(RevercedCb);
            printMatrix(RevercedCr);

            const auto [R,G,B] = convertYCbCrToRGB_AL(RevercedY, RevercedCb, RevercedCr);

            {
                constexpr const auto HalfRowsCount = 8;
                constexpr const auto HalfColsCount = 8;

                constexpr const auto ColsCount = 16;
                constexpr const auto RowsCount = 16;


                std::unique_ptr<TIFF, std::function<void(TIFF* const)>> Image{
                      TIFFOpen("outout_tiff_in2v.tif", "w")
                    , []( TIFF * const image){
                      TIFFClose(image);
                    }
                };

                TIFFSetField(Image.get(), TIFFTAG_IMAGEWIDTH, ColsCount);
                TIFFSetField(Image.get(), TIFFTAG_IMAGELENGTH, RowsCount);
                TIFFSetField(Image.get(), TIFFTAG_BITSPERSAMPLE, 8);
                TIFFSetField(Image.get(), TIFFTAG_SAMPLESPERPIXEL, 3);
                TIFFSetField(Image.get(), TIFFTAG_ROWSPERSTRIP, 1);
                TIFFSetField(Image.get(), TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
                TIFFSetField(Image.get(), TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
                TIFFSetField(Image.get(), TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB); //
                TIFFSetField(Image.get(), TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT );
                TIFFSetField(Image.get(), TIFFTAG_COMPRESSION, COMPRESSION_NONE);

                std::array<uint8, ColsCount * 3> ScanLine;

                for ( std::size_t Row = 0; Row < RowsCount; Row++) {
                  auto It = std::begin(ScanLine);
                  for ( std::size_t Col = 0; Col < ColsCount; ++Col ) {
                      *It = R(Row,Col);
                      *(It + 1) = G(Row,Col);
                      *(It + 2) = B(Row,Col);
                      It += 3;
                  }
                  TIFFWriteScanline(Image.get(), &ScanLine[0], static_cast<uint32>(Row), 0);
                }
            }

            Context::RGB r;
            r.R = R;
            r.G = G;
            r.B = B;
            Ctx.Image.push_back(r);
        }
    } while (false);
    std::cout << "while (false)" << std::endl;
}

