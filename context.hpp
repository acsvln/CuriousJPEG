#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <memory>
#include <optional>
#include <vector>

#include <boost/numeric/ublas/matrix.hpp>

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

struct DHTNode
{
    std::shared_ptr<DHTNode> left = nullptr;
    std::shared_ptr<DHTNode> right = nullptr;
    std::weak_ptr<DHTNode> parent;
    std::optional<uint8_t> data = {};

    bool IsLeaf() const {
        return data.has_value();
    }

};

struct Context
{
    std::vector<DQTMatrix> DQT_Vector;
    std::vector<std::shared_ptr<DHTNode>> AC_HuffmanTables;
    std::vector<std::shared_ptr<DHTNode>> DC_HuffmanTables;
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
