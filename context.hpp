#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <memory>
#include <optional>
#include <vector>

#include <boost/numeric/ublas/matrix.hpp>

#include "huffman_tree.hpp"

using DQTMatrix = boost::numeric::ublas::matrix<uint16_t>;

struct DCTComponent
{
    uint8_t id:8;
    uint8_t h:4;
    uint8_t v:4;
    uint8_t dqtId:8;
};

struct DCTTable
{
    uint8_t precision;
    uint32_t height;
    uint32_t width;
    uint32_t maxH;
    uint32_t maxV;
    std::vector<DCTComponent> components;
};



struct Context
{
    std::vector<DQTMatrix> DQT_Vector;
    std::vector<std::shared_ptr<HuffmanTree::Node>> AC_HuffmanTables;
    std::vector<std::shared_ptr<HuffmanTree::Node>> DC_HuffmanTables;
    struct RGB {
        using Matr = boost::numeric::ublas::matrix<uint16_t>;
        Matr R;
        Matr G;
        Matr B;
    };

    std::vector<RGB> Image;
    DCTTable dct;
};

#endif // CONTEXT_HPP
