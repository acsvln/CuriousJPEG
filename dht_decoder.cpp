#include "dht_decoder.hpp"

#include <sstream>

#include "data_reader.hpp"
#include "huffman_tree.hpp"
#include "utility.hpp"

namespace {

constexpr std::size_t HuffmanCodeCount = 16;
constexpr uint8_t HuffmanClassDC = 0u;
constexpr uint8_t HuffmanClassAC = 1u;

auto invalidTableClassException(uint8_t const TableClass) {
  std::stringstream ss;
  ss << "Invalid table class " << TableClass;
  return std::runtime_error{ss.str()};
}

} // namespace

DHTDecoder::DHTDecoder() : Decoder{"Define Huffman Table"} {}

void DHTDecoder::InvokeImpl(std::istream &Stream, Context &Ctx) {

  const auto huffmanTableVectorForClass = [&](uint8_t const TableClass)
      -> std::vector<std::shared_ptr<HuffmanTree::Node>> & {
    switch (TableClass) {
    case HuffmanClassDC:
      return Ctx.DC_HuffmanTables;
    case HuffmanClassAC:
      return Ctx.AC_HuffmanTables;
    }

    throw invalidTableClassException(TableClass);
  };

  const auto TableDescription = DataReader::readNumber<uint8_t>(Stream);
  const auto TableClass = lowByte(TableDescription);
  const auto TableIndex = highByte(TableDescription);

  auto &HuffmanVector = huffmanTableVectorForClass(TableClass);
  if (HuffmanVector.size() <= TableIndex) {
    HuffmanVector.resize(TableIndex + 1);
  }

  const auto root = std::make_shared<HuffmanTree::Node>(nullptr);

  std::array<uint8_t, HuffmanCodeCount> HuffmanCodes;
  DataReader::readBuffer(Stream, HuffmanCodes);

  for (std::size_t index = 0; index < HuffmanCodes.size(); ++index) {
    const auto Count = HuffmanCodes[index];
    if (0 == Count) {
      continue;
    }

    std::vector<uint8_t> HuffmanData(Count);
    DataReader::readBuffer(Stream, HuffmanData);

    for (const auto code : HuffmanData) {
      HuffmanTree::createAndinsertNode(root, index, code);
    }
  }

  HuffmanVector[TableIndex] = root;
}
