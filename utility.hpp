#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <cstddef>
#include <string>

#include <boost/endian/conversion.hpp>

// TODO: remove that helper
template <typename T>
        T swap_endian(T u)
{
    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (std::size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

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
