#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <boost/numeric/ublas/matrix.hpp>

struct Context
{
    using QuantTable = boost::numeric::ublas::matrix<uint16_t>;

    std::vector<QuantTable> quantTables;

    int dummy = 0;
};

#endif // CONTEXT_HPP
