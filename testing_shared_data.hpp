#ifndef TESTING_SHARED_DATA_HPP
#define TESTING_SHARED_DATA_HPP

#include <boost/numeric/ublas/matrix.hpp>
#include <stdint.h>

auto DefineQuantizationTable1() -> boost::numeric::ublas::matrix<uint16_t>;
auto DefineQuantizationTable2() -> boost::numeric::ublas::matrix<uint16_t>;

#endif // TESTING_SHARED_DATA_HPP
