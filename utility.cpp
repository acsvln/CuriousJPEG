#include "utility.hpp"

#include <iostream>
#include <array>

#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>

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

std::ostream &boost::numeric::ublas::boost_test_print_type(std::ostream &ostr, const boost::numeric::ublas::matrix<uint16_t> &right) {
    ostr << right;
    return ostr;
}

bool boost::numeric::ublas::operator==(const boost::numeric::ublas::matrix<uint16_t> &left, const boost::numeric::ublas::matrix<uint16_t> &right) {
    using size_type = boost::numeric::ublas::matrix<uint16_t>::size_type;

    if (left.size1() != right.size1()) {
        return false;
    }

    if (left.size2() != right.size2()){
        return false;
    }

    for(size_type i=0;i< left.size1();++i)
    {
        for (size_type j=0;j<left.size2();++j)
        {
            if (left(i,j) != right(i,j)) {
                return false;
            }
        }
    }

    return true;
}
