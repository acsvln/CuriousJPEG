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

///////////////////////////////////////

// TODO: вынести в отдельный модуль для тестов
namespace boost::numeric::ublas {

std::ostream& boost_test_print_type(std::ostream& ostr, boost::numeric::ublas::matrix<uint16_t> const& right);

bool operator==( const boost::numeric::ublas::matrix<uint16_t>& left, const boost::numeric::ublas::matrix<uint16_t>& right );

}

#endif // UTILITY_HPP
