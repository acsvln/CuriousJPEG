#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <fstream>
#include <iostream>
#include <array>
#include <boost/iostreams/stream.hpp>

#include "testing_utility.hpp"

using namespace boost::iostreams;
using namespace std;

#include "dqt_decoder.hpp"

void prnt_matr666(const boost::numeric::ublas::matrix<uint8_t> &matrix) {
  std::cout << std::endl << "prnt_matr" << std::endl;

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      std::cout << std::hex << (int)matrix(i, j) << '\t';
    }
    std::cout << std::endl;
  }
}


void prnt_matr2( const boost::numeric::ublas::matrix<uint8_t>& matrix ) {
    std::cout << std::endl << "prnt_matr XXXXX" << std::endl;

    for ( int i = 0; i < 8; i++ ) {
        for ( int j = 0; j < 8; j++ ) {
            std::cout << "i" << i << "j" << j << std::endl;
            std::cout << (int) matrix( i, j ) << '\t';
        }
        std::cout << std::endl;
    }

}

BOOST_AUTO_TEST_SUITE(DQTDecoderTests)

BOOST_AUTO_TEST_CASE(Invoke) {
    const std::array<char, 67> source = {
        (char)0x00, (char)0x43, (char)0x00, (char)0xA0, (char)0x6E, (char)0x78, (char)0x8c,
        (char)0x78, (char)0x64, (char)0xA0, (char)0x8c, (char)0x82, (char)0x8c, (char)0xb4, (char)0xaa,
        (char)0xa0, (char)0xbe, (char)0xf0, (char)0xFF, (char)0xFF, (char)0xF0, (char)0xDC, (char)0xDC,
        (char)0xF0, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF,
        (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF,
        (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF,
        (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF,
        (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF,
        (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF
    };

    boost::numeric::ublas::matrix<uint16_t> expected(8, 8);
    expected <<=    0xA0, 0x6E, 0x64, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF,
                    0x78, 0x78, 0x8C, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF,
                    0x8C, 0x82, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF,
                    0x8C, 0xAA, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xB4, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF;


    Context context;

    DQTDecoder decoder;

    basic_array_source<char> input_source(source.data(), source.size());
    stream<basic_array_source<char>> input_stream(input_source);
    decoder.Invoke(input_stream, context);

    std::cout << "-------------------------------------------------" << std::endl;
    prnt_matr666(context.DQT_Vector[0]);
    prnt_matr666(expected);


    BOOST_CHECK_EQUAL(context.DQT_Vector.size(), 1 );
    BOOST_CHECK_EQUAL(expected,  context.DQT_Vector[0]);
}


BOOST_AUTO_TEST_CASE(Invoke_1) {
    const std::array<char, 67> source = {
        (char)0x00, (char)0x43, (char)0x01, (char)0xaa, (char)0xb4,
        (char)0xb4, (char)0xf0, (char)0xd2, (char)0xf0, (char)0xff, (char)0xff, (char)0xff,
        (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
        (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
        (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
        (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
        (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
        (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
        (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff,
        (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff, (char)0xff
    };


    boost::numeric::ublas::matrix<uint16_t> expected(8, 8);
    expected <<=    0xaa, 0xb4, 0xf0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xb4, 0xd2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xf0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF;


    Context context;

    DQTDecoder decoder;

    basic_array_source<char> input_source(source.data(), source.size());
    stream<basic_array_source<char>> input_stream(input_source);
    decoder.Invoke(input_stream, context);

    BOOST_CHECK_EQUAL(context.DQT_Vector.size(), 2 );
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    prnt_matr666(context.DQT_Vector[1]);
    BOOST_CHECK_EQUAL(expected,  context.DQT_Vector[1]);


}

BOOST_AUTO_TEST_SUITE_END()
