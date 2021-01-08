#ifndef TESTING_SHARED_DATA_HPP
#define TESTING_SHARED_DATA_HPP

#include <boost/numeric/ublas/matrix.hpp>
#include <stdint.h>

#include "testing_utility.hpp"

auto DefineQuantizationTable1() -> MatrixT<uint16_t>;
auto DefineQuantizationTable2() -> MatrixT<uint16_t>;

#endif // TESTING_SHARED_DATA_HPP
