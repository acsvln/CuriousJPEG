#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstddef>
#include <string>

#include <boost/endian/conversion.hpp>

template<class NumberType, class StreamType>
NumberType ReadNumberFromStream( StreamType& aStream )
{
    namespace be = boost::endian;

    NumberType buffer;
    aStream.read(reinterpret_cast<char*>(&buffer),sizeof(buffer));
    return be::native_to_big(buffer);
}

void printSectionDescription(const std::string& name, std::size_t size);

#endif // UTILITY_HPP
