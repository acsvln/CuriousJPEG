#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <boost/numeric/ublas/matrix.hpp>

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
    using QuantTable = boost::numeric::ublas::matrix<uint16_t>;

    std::vector<QuantTable> quantTables;
    DCTTable dct;

    int dummy = 0;
};

#endif // CONTEXT_HPP
