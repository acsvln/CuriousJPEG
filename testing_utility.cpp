#include "testing_utility.hpp"

#include <boost/numeric/ublas/io.hpp>
#include <iostream>
#include <vector>

namespace boost::numeric::ublas {

namespace {
template <class T>
auto comparsionImpl(const matrix<T> &Left, const matrix<T> &Right) -> bool {
  using SizeType = typename matrix<T>::size_type;

  if (Left.size1() != Right.size1()) {
    return false;
  }

  if (Left.size2() != Right.size2()) {
    return false;
  }

  for (SizeType I = 0; I < Left.size1(); ++I) {
    for (SizeType J = 0; J < Left.size2(); ++J) {
      if (Left(I, J) != Right(I, J)) {
        return false;
      }
    }
  }

  return true;
}
auto comparsionImpl(const matrix<double> &Left, const matrix<double> &Right)
    -> bool = delete;
} // namespace

bool operator==(const matrix<uint16_t> &Left, const matrix<uint16_t> &Right) {
  return comparsionImpl(Left, Right);
}
bool operator==(const matrix<uint8_t> &Left, const matrix<uint8_t> &Right) {
  return comparsionImpl(Left, Right);
}
auto operator==(const matrix<int16_t> &Left, const matrix<int16_t> &Right)
    -> bool {
  return comparsionImpl(Left, Right);
}
auto operator==(const matrix<int8_t> &Left, const matrix<int8_t> &Right)
    -> bool {
  return comparsionImpl(Left, Right);
}
} // namespace boost::numeric::ublas



void printVector(const std::array<int16_t, 64> &Vector) {
  std::cout << Vector.size() << ':' << std::endl;
  for (const auto &Element : Vector) {
    std::cout << Element << '\t';
  }
  std::cout << std::endl;
}
