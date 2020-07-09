#include "testing_utility.hpp"

#include <iostream>

#include <boost/numeric/ublas/io.hpp>

namespace boost::numeric::ublas {

namespace {
template <class T>
auto comparsionImpl(matrix<T> const &left, matrix<T> const &right) -> bool {
  using size_type = typename matrix<T>::size_type;

  if (left.size1() != right.size1()) {
      std::cout << "111" << std::endl;
    return false;
  }

  if (left.size2() != right.size2()) {
      std::cout << "222" << std::endl;
    return false;
  }

  for (size_type i = 0; i < left.size1(); ++i) {
    for (size_type j = 0; j < left.size2(); ++j) {
      if (left(i, j) != right(i, j)) {
          std::cout << "_" << left(i, j) << "_" <<  right(i, j) << std::endl;
        return false;
      }
    }
  }

  std::cout << "544" << std::endl;
  return true;
}
} // namespace

bool operator==(matrix<uint16_t> const &left, matrix<uint16_t> const &right) {
  return comparsionImpl(left, right);
}
bool operator==(matrix<uint8_t> const &left, matrix<uint8_t> const &right) {
  return comparsionImpl(left, right);
}
auto operator==(matrix<int16_t> const &left, matrix<int16_t> const &right)
    -> bool {
  return comparsionImpl(left, right);
}
} // namespace boost::numeric::ublas
