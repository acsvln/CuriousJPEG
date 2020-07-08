#ifndef TESTING_UTILITY_H
#define TESTING_UTILITY_H

#include <boost/numeric/ublas/matrix.hpp>
#include <ostream>

namespace boost::numeric::ublas {
bool operator==(matrix<uint16_t> const &left, matrix<uint16_t> const &right);
}

#endif // TESTING_UTILITY_H
