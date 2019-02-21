#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstddef>
#include <boost/numeric/ublas/matrix.hpp>
#include <experimental/optional>

// TODO: remove that helper
template <typename T>
        T swap_endian(T u)
{
    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (std::size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

std::experimental::optional<std::pair<unsigned,unsigned>> AddToZigZagMatrix(boost::numeric::ublas::matrix<uint16_t> matrix, std::pair<unsigned,unsigned> position, uint16_t value);


#endif // UTILITY_HPP
