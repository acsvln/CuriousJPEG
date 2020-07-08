#include "testing_utility.hpp"

#include <boost/numeric/ublas/io.hpp>

namespace boost::numeric::ublas {
bool operator==(matrix<uint16_t> const &left, matrix<uint16_t> const &right) {
  using size_type = matrix<uint16_t>::size_type;

  if (left.size1() != right.size1()) {
    return false;
  }

  if (left.size2() != right.size2()) {
    return false;
  }

  for (size_type i = 0; i < left.size1(); ++i) {
    for (size_type j = 0; j < left.size2(); ++j) {
      if (left(i, j) != right(i, j)) {
        return false;
      }
    }
  }

  return true;
}
} // namespace boost::numeric::ublas
