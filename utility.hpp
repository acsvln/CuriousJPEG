#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstddef>
#include <string>
#include <array>

#include <boost/numeric/ublas/matrix.hpp>

template<class To>
class TransformCaster {
public:
    template<class From>
    auto operator()( From const from ) -> To {
        return static_cast<To>(from);
    }
};

auto lowByte( uint8_t const Src ) -> uint8_t;
auto highByte( uint8_t const Src ) -> uint8_t;

boost::numeric::ublas::matrix<uint16_t> CreateZigZagMatrix( std::array<uint16_t, 64> source );

//-------------------------------------
//-------------------------------------
//-------------------------------------
void printSectionDescription(const std::string& name, std::size_t size);

#endif // UTILITY_HPP
