﻿#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <memory>
#include <optional>
#include <vector>

#include <boost/numeric/ublas/matrix.hpp>

#include "huffman_tree.hpp"

using DataUnit = boost::numeric::ublas::matrix<int16_t>;

struct MinimumCodedUnit {
  std::vector<DataUnit> Cs1;
  std::vector<DataUnit> Cs2;
  std::vector<DataUnit> Cs3;
  std::vector<DataUnit> Cs4;
};

using DQTMatrix = boost::numeric::ublas::matrix<uint16_t>;

struct DCTComponent
{
    uint8_t Id:8;
    uint8_t H:4;
    uint8_t V:4;
    uint8_t DQT_Id:8;
};

struct DCTTable
{
    uint8_t Precision;
    uint32_t Height;
    uint32_t Width;
    uint32_t MaxH;
    uint32_t MaxV;
    std::vector<DCTComponent> Components;
};

struct Context
{
    std::vector<DQTMatrix> DQT_Vector;
    std::vector<std::shared_ptr<HuffmanTree::Node>> AC_HuffmanTables;
    std::vector<std::shared_ptr<HuffmanTree::Node>> DC_HuffmanTables;
    struct RGB {
        using Matr = boost::numeric::ublas::matrix<uint8_t>;
        Matr R;
        Matr G;
        Matr B;
    };

 MinimumCodedUnit mcu;

    std::vector<RGB> Image;
    DCTTable dct;
};

#endif // CONTEXT_HPP
