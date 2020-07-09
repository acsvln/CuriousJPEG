#include "dht_decoder.hpp"

#include <iostream>

#include "data_reader.hpp"
#include "huffman_tree.hpp"

DHTDecoder::DHTDecoder()
    : Decoder{"Define Huffman Table"}
{}

void DHTDecoder::InvokeImpl(std::istream &aStream, Context &aContext) {
  static constexpr std::size_t HuffCodeCount = 16;
  static constexpr uint8_t HuffClassDC = 0u;
  static constexpr uint8_t HuffClassAC = 1u;

  const auto extractHuffTableVector =
      [&](const uint8_t tableClass) -> std::vector<std::shared_ptr<HuffmanTree::Node>> & {
    switch (tableClass) {
    case HuffClassDC:
      return aContext.DC_HuffmanTables;
    case HuffClassAC:
      return aContext.AC_HuffmanTables;
    }

    std::stringstream ss;
    ss << "Invalid table class " << tableClass;
    throw std::runtime_error(ss.str());
  };

  const auto description = DataReader::readNumber<uint8_t>(aStream);
  const uint8_t tableClass = lowByte( description );
  const uint8_t tableIndex = highByte( description );

  auto &huffVector = extractHuffTableVector(tableClass);

  if (huffVector.size() <= tableIndex) {
    huffVector.resize(tableIndex + 1);
  }

  const auto root = std::make_shared<HuffmanTree::Node>( nullptr );

  std::array<uint8_t, HuffCodeCount> huffCodes;


  DataReader::readBuffer( aStream, huffCodes );

  for (std::size_t index = 0; index < huffCodes.size(); ++index) {
    uint8_t count = huffCodes[index];
    if (count <= 0) {
      continue;
    }

    std::vector<uint8_t> huffData;
    huffData.resize(count);
    DataReader::readBuffer( aStream, huffData );

    for (uint8_t code : huffData) {
      HuffmanTree::createAndinsertNode(root, index, code);
    }
  }

  huffVector[tableIndex] = root;
}
