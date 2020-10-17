#include "utility.hpp"

#include <array>

auto highByte(uint8_t const Src) -> uint8_t { return Src >> 4; }

auto lowByte(uint8_t const Src) -> uint8_t { return Src & 0xF; }

template <class Type>
auto CreateZigZagMatrixImpl(std::array<Type, 64> Source)
    -> boost::numeric::ublas::matrix<Type> {
  using SizeType = typename boost::numeric::ublas::matrix<Type>::size_type;
  SizeType const Size = 8;
  boost::numeric::ublas::matrix<Type> Matrix(Size, Size);

  auto StartIterator = Source.begin();
  auto EndIterator = Source.rbegin();

  for (SizeType I = 0; I < Size; I++) {
    SizeType X = I;
    if (I % 2) { // нечетные
      for (SizeType J = 0; (J <= I) && (StartIterator < Source.end()) &&
                           (EndIterator < Source.rend());
           J++) {
        Matrix(J, X) = *StartIterator;
        Matrix(Size - 1 - J, Size - 1 - X) = *EndIterator;
        ++StartIterator;
        ++EndIterator;

        if (0 == X) {
          break;
        } else {
          X--;
        }
      }
    } else { // четные
      for (SizeType J = 0; (J <= I) && (StartIterator < Source.end()) &&
                           (EndIterator < Source.rend());
           J++) {
        Matrix(X, J) = *StartIterator;
        Matrix(Size - 1 - X, Size - 1 - J) = *EndIterator;
        ++StartIterator;
        ++EndIterator;

        if (0 == X) {
          break;
        } else {
          X--;
        }
      }
    }
  }

  return Matrix;
}

auto CreateZigZagMatrix(std::array<int16_t, 64> source)
    -> boost::numeric::ublas::matrix<int16_t> {
  return CreateZigZagMatrixImpl(source);
}

auto CreateZigZagMatrix(std::array<uint16_t, 64> source)
    -> boost::numeric::ublas::matrix<uint16_t> {
  return CreateZigZagMatrixImpl(source);
}
