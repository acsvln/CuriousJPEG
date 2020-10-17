#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <array>
#include <cstddef>

#include <boost/numeric/ublas/matrix.hpp>

template <class To> class TransformCaster {
public:
  template <class From> auto operator()(From const from) -> To {
    return static_cast<To>(from);
  }
};

auto highByte(uint8_t const Src) -> uint8_t;
auto lowByte(uint8_t const Src) -> uint8_t;

auto CreateZigZagMatrix(std::array<uint16_t, 64> Source)
    -> boost::numeric::ublas::matrix<uint16_t>;
auto CreateZigZagMatrix(std::array<int16_t, 64> source)
    -> boost::numeric::ublas::matrix<int16_t>;

#endif // UTILITY_HPP
