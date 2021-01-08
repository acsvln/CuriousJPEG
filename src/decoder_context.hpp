#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <boost/numeric/ublas/matrix.hpp>
#include <memory>
#include <vector>

#include "huffman_tree.hpp"

using DQTMatrix = boost::numeric::ublas::matrix<uint16_t>;

struct DCTComponent final {
  uint8_t Id : 8;
  uint8_t H : 4;
  uint8_t V : 4;
  uint8_t DQT_Id : 8;
};

struct DCTTable final {
  uint8_t Precision;
  uint32_t Height;
  uint32_t Width;
  uint32_t MaxH;
  uint32_t MaxV;
  std::vector<DCTComponent> Components;
};

struct RGBBlock final {
  using Channel = boost::numeric::ublas::matrix<uint8_t>;

  Channel R;
  Channel G;
  Channel B;
};

struct DecoderContext final {
  std::vector<DQTMatrix> DQT_Vector;
  std::vector<std::shared_ptr<HuffmanTree::Node>> AC_HuffmanTables;
  std::vector<std::shared_ptr<HuffmanTree::Node>> DC_HuffmanTables;
  DCTTable dct;
  std::vector<RGBBlock> Image;
};

#endif // CONTEXT_HPP
