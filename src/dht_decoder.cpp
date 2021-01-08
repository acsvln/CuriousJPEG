#include "dht_decoder.hpp"

#include <sstream>

#include "data_reader.hpp"
#include "exceptions.hpp"
#include "huffman_tree.hpp"
#include "utility.hpp"

namespace {

constexpr std::size_t HuffmanCodeCount = 16;
constexpr uint8_t HuffmanClassDC = 0u;
constexpr uint8_t HuffmanClassAC = 1u;

} // namespace

DHTDecoder::DHTDecoder() : DecoderBase{"Define Huffman Table"} {}

void DHTDecoder::InvokeImpl(std::istream &Stream, DecoderContext &Context) {

  const auto huffmanTableVectorForClass = [&](const uint8_t TableClass)
      -> std::vector<std::shared_ptr<HuffmanTree::Node>> & {
    switch (TableClass) {
    case HuffmanClassDC:
      return Context.DC_HuffmanTables;
    case HuffmanClassAC:
      return Context.AC_HuffmanTables;
    }
    throw InvalidJPEGDataException{"Wrong Table class " +
                                   std::to_string(TableClass)};
  };

  const auto TableDescription = DataReader::readNumber<uint8_t>(Stream);
  const auto TableClass = highByte(TableDescription);
  const auto TableIndex = lowByte(TableDescription);

  auto &HuffmanVector = huffmanTableVectorForClass(TableClass);
  if (HuffmanVector.size() <= TableIndex) {
    HuffmanVector.resize(TableIndex + 1);
  }

  const auto Root = std::make_shared<HuffmanTree::Node>(nullptr);

  std::array<uint8_t, HuffmanCodeCount> HuffmanCodes;
  DataReader::readBuffer(Stream, HuffmanCodes);

  for (std::size_t Index = 0; Index < HuffmanCodes.size(); ++Index) {
    const auto Count = HuffmanCodes[Index];
    if (0 == Count) {
      continue;
    }

    std::vector<uint8_t> HuffmanData(Count);
    DataReader::readBuffer(Stream, HuffmanData);

    for (const auto Code : HuffmanData) {
      HuffmanTree::createAndInsertNode(Root, Index, Code);
    }
  }

  HuffmanVector[TableIndex] = Root;
}
