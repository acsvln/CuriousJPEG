#ifndef TESTING_UTILITY_H
#define TESTING_UTILITY_H

#include <boost/numeric/ublas/matrix.hpp>
#include <ostream>

namespace boost::numeric::ublas {
auto operator==(matrix<uint16_t> const &left, matrix<uint16_t> const &right)
    -> bool;
// TODO: Maybe fix
auto operator==(matrix<uint8_t> const &left, matrix<uint8_t> const &right)
    -> bool;
auto operator==(matrix<int16_t> const &left, matrix<int16_t> const &right)
    -> bool;
} // namespace boost::numeric::ublas

#endif // TESTING_UTILITY_H
