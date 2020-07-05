#include "dht_decoder.hpp"

#include <iostream>

#include "huffman_tree.h"

void DHTDecoder::Invoke(std::istream &aStream, Context &aContext) {
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

  const auto size = ReadNumberFromStream<uint16_t>(aStream);
  printSectionDescription("Define Huffman Table", size);

  const auto description = ReadNumberFromStream<uint8_t>(aStream);
  const uint8_t tableClass = description >> 4;
  const uint8_t tableIndex = description & 0xF;

  auto &huffVector = extractHuffTableVector(tableClass);

  if (huffVector.size() <= tableIndex) {
    huffVector.resize(tableIndex + 1);
  }

  const auto root = std::make_shared<HuffmanTree::Node>( nullptr );

  std::array<uint8_t, HuffCodeCount> huffCodes;

  static_assert(sizeof(std::istream::char_type) ==
                sizeof(std::array<uint8_t, HuffCodeCount>::value_type));
  static_assert(sizeof(std::istream::char_type *) ==
                sizeof(std::array<uint8_t, HuffCodeCount>::pointer));
  aStream.read(reinterpret_cast<std::istream::char_type *>(huffCodes.data()),
               sizeof(huffCodes));

  for (std::size_t index = 0; index < huffCodes.size(); ++index) {
    uint8_t count = huffCodes[index];
    if (count <= 0) {
      continue;
    }

    std::vector<uint8_t> huffData;
    huffData.resize(count);
    aStream.read(reinterpret_cast<std::istream::char_type *>(huffData.data()),
                 huffData.size() * sizeof(uint8_t));

    for (uint8_t code : huffData) {
      HuffmanTree::createAndinsertNode(root, index, code);
    }
  }

  huffVector[tableIndex] = root;
}
