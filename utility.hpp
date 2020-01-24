#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstddef>

#include <boost/endian/conversion.hpp>

template<class NumberType, class StreamType>
NumberType ReadNumberFromStream( StreamType& aStream )
{
    namespace be = boost::endian;

    NumberType buffer;
    aStream.read(reinterpret_cast<char*>(&buffer),sizeof(buffer));
    return be::native_to_big(buffer);
}


#endif // UTILITY_HPP
