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

boost::numeric::ublas::matrix<uint16_t> CreateZigZagMatrix( std::array<uint16_t, 64> source )
{
    boost::numeric::ublas::matrix<uint16_t> matrix(SIZE,SIZE);

    auto start_iterator = source.begin();
    auto end_iterator = source.rbegin();

    for (uint16_t i = 0; i < SIZE; i++)
    {
        uint16_t x = i;
        if (i % 2) { // нечетные
            for (uint16_t j = 0;
                 (j <= i) && (start_iterator < source.end()) && (end_iterator < source.rend());
                 j++, x--)
            {
                matrix(j,x) = *start_iterator;
                matrix(SIZE - 1 - j, SIZE - 1 - x) = *end_iterator;
                ++start_iterator;
                ++end_iterator;
            }
        }
        else // четные
            for (uint16_t j = 0;
                 (j <= i) && (start_iterator < source.end()) && (end_iterator < source.rend())
                 ; j++, x--)
            {
                matrix(x,j) = *start_iterator;
                matrix(SIZE - 1 - x, SIZE - 1 - j) = *end_iterator;
                ++start_iterator;
                ++end_iterator;
            }
    }

    return matrix;
}

