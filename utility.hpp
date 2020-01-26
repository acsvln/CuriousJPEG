#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstddef>
#include <string>
#include <array>

#include <boost/endian/conversion.hpp>
#include <boost/numeric/ublas/matrix.hpp>

template<class NumberType, class StreamType>
NumberType ReadNumberFromStream( StreamType& aStream )
{
    namespace be = boost::endian;

    NumberType buffer;
    aStream.read(reinterpret_cast<char*>(&buffer),sizeof(buffer));
    return be::native_to_big(buffer);
}

void printSectionDescription(const std::string& name, std::size_t size);

boost::numeric::ublas::matrix<uint16_t> CreateZigZagMatrix( std::array<uint16_t, 64> source );

#endif // UTILITY_HPP
