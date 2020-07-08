#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstddef>
#include <string>
#include <array>

#include <boost/endian/conversion.hpp>
#include <boost/numeric/ublas/matrix.hpp>

auto lowByte( uint8_t const Src ) -> uint8_t;
auto highByte( uint8_t const Src ) -> uint8_t;

void printSectionDescription(const std::string& name, std::size_t size);

boost::numeric::ublas::matrix<uint16_t> CreateZigZagMatrix( std::array<uint16_t, 64> source );

#endif // UTILITY_HPP
