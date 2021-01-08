#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <array>
#include <cstddef>

#include <boost/numeric/ublas/matrix.hpp>

template <class To> class TransformCaster {
public:
  template <class From> auto operator()(const From from) -> To {
    return static_cast<To>(from);
  }
};

auto highByte(const uint8_t Number) -> uint8_t;
auto lowByte(const uint8_t Number) -> uint8_t;

auto CreateZigZagMatrix(std::array<uint16_t, 64> Source)
    -> boost::numeric::ublas::matrix<uint16_t>;
auto CreateZigZagMatrix(std::array<int16_t, 64> source)
    -> boost::numeric::ublas::matrix<int16_t>;

void saveRGBToImage(const boost::numeric::ublas::matrix<uint8_t> &R,
                    const boost::numeric::ublas::matrix<uint8_t> &G,
                    const boost::numeric::ublas::matrix<uint8_t> &B,
                    const std::string &path);

#endif // UTILITY_HPP
