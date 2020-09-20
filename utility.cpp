#include "utility.hpp"

#include <iostream>
#include <array>

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/numeric/ublas/assignment.hpp>

auto lowByte( uint8_t const Src ) -> uint8_t {
    return Src >> 4;
}

auto highByte( uint8_t const Src ) -> uint8_t {
    return Src & 0xF;
}

static const uint16_t SIZE = 8;

void printSectionDescription(const std::string &name, std::size_t size)
{
    std::cout << "Segment started: " << name << std::endl;
    std::cout << "Segment size: " << size << " bytes" << std::endl;
}

template <class Type>
auto CreateZigZagMatrixImpl(std::array<Type, 64> Source)
    -> boost::numeric::ublas::matrix<Type> {
  boost::numeric::ublas::matrix<Type> matrix(SIZE, SIZE);

  auto start_iterator = Source.begin();
  auto end_iterator = Source.rbegin();

  for (uint16_t i = 0; i < SIZE; i++) {
    uint16_t x = i;
    if (i % 2) { // нечетные
      for (uint16_t j = 0; (j <= i) && (start_iterator < Source.end()) &&
                           (end_iterator < Source.rend());
           j++, x--) {
        matrix(j, x) = *start_iterator;
        matrix(SIZE - 1 - j, SIZE - 1 - x) = *end_iterator;
        ++start_iterator;
        ++end_iterator;
      }
    } else // четные
      for (uint16_t j = 0; (j <= i) && (start_iterator < Source.end()) &&
                           (end_iterator < Source.rend());
           j++, x--) {
        matrix(x, j) = *start_iterator;
        matrix(SIZE - 1 - x, SIZE - 1 - j) = *end_iterator;
        ++start_iterator;
        ++end_iterator;
      }
  }

  return matrix;
}

auto CreateZigZagMatrix(std::array<int16_t, 64> source)
    -> boost::numeric::ublas::matrix<int16_t> {
  return CreateZigZagMatrixImpl(source);
}

auto CreateZigZagMatrix(std::array<uint16_t, 64> source)
    -> boost::numeric::ublas::matrix<uint16_t> {
  return CreateZigZagMatrixImpl(source);
}
