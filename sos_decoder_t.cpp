#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <fstream>
#include <iostream>
#include <array>

using namespace boost::iostreams;
using namespace std;

#include "sos_decoder.hpp"

BOOST_AUTO_TEST_SUITE(SOSDecoderTests)

BOOST_AUTO_TEST_CASE(BitExtractor) {
    const std::array<char, 4> data = {
        (char)0b10101001, 0b01111011, 0b01100101, 0b00000001
    };

    basic_array_source<char> input_source(data.data(), data.size());
    stream<basic_array_source<char>> input_stream(input_source);

    SOSDecoder::BitExtractor extractor{ input_stream };

    // 0b10101001
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);

    // 0b01111011
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 1);

    // 0b01100101
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);

    // 0b00000001
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
}

BOOST_AUTO_TEST_CASE(BitExtractor_WithLength) {
    const std::array<char, 4> data = {
        (char)0b10101001, 0b01111011, 0b01100101, 0b00000001
    };

    basic_array_source<char> input_source(data.data(), data.size());
    stream<basic_array_source<char>> input_stream(input_source);

    SOSDecoder::BitExtractor extractor{ input_stream };

    //-------------------------------------
    BOOST_REQUIRE(extractor.nextNumber( 6 ) == 0b101010);
    BOOST_REQUIRE(extractor.nextNumber( 7 ) == 0b0101111);
    BOOST_REQUIRE(extractor.nextNumber( 8 ) == 0b01101100);
    BOOST_REQUIRE(extractor.nextNumber( 5 ) == 0b10100);
    BOOST_REQUIRE(extractor.nextNumber( 2 ) == 0b00);
    BOOST_REQUIRE(extractor.nextNumber( 4 ) == 0b0001);
}

BOOST_AUTO_TEST_CASE(ReadTable_Y1) {
    boost::numeric::ublas::matrix<char> expected(8, 8);
    expected <<=    2,  0,  3, 0, 0, 0, 0, 0,
                    0,  1,  2, 0, 0, 0, 0, 0,
                    0, -1, -1, 0, 0, 0, 0, 0,
                    1,  0,  0, 0, 0, 0, 0, 0,
                    0,  0,  0, 0, 0, 0, 0, 0,
                    0,  0,  0, 0, 0, 0, 0, 0,
                    0,  0,  0, 0, 0, 0, 0, 0,
                    0,  0,  0, 0, 0, 0, 0, 0;
    //     0

    const std::array<char, 5> data = {
        (char)0b10101110, (char)0b11100111, 0b01100001, (char)0b11110010, 0b0
    };

    basic_array_source<char> input_source(data.data(), data.size());
    stream<basic_array_source<char>> input_stream(input_source);

    SOSDecoder::BitExtractor extractor{ input_stream };

    //-------------------------------------
    std::shared_ptr<DHTNode> AC_HuffmanTables = {

    };
    std::shared_ptr<DHTNode> DC_HuffmanTables = {

    };

    //-------------------------------------

}

BOOST_AUTO_TEST_CASE(Invoke) {
//    const std::array<char, 31> data = {
//        (char)0x00, (char)0x0c, (char)0x03, (char)0x01, (char)0x00, (char)0x02,
//        (char)0x11, (char)0x03, (char)0x11, (char)0x00, (char)0x3f, (char)0x00,
//        (char)0xae, (char)0xe7, (char)0x61, (char)0xf2, (char)0x1b, (char)0xd5,
//        (char)0x22, (char)0x85, (char)0x5d, (char)0x04, (char)0x3c, (char)0x82,
//        (char)0xc8, (char)0x48, (char)0xb1, (char)0xdc, (char)0xbf, (char)0xff,
//        (char)0xd9
//     };

//    basic_array_source<char> input_source(data.data(), data.size());
//    stream<basic_array_source<char>> input_stream(input_source);

//    Context context;
//    SOSDecoder decoder;

//    decoder.Invoke(input_stream, context);
}

BOOST_AUTO_TEST_SUITE_END()
