#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <fstream>
#include <iostream>
using namespace boost::iostreams;
using namespace std;

#include "sof0_decoder.hpp"

BOOST_AUTO_TEST_SUITE(SOF0DecoderTests)

BOOST_AUTO_TEST_CASE(Invoke) {
    const std::array<char, 17> data = {   0x00, 0x11, 0x08, 0x00, 0x10, 0x00, 0x10, 0x03,
                                          0x01, 0x22, 0x00,
                                          0x02, 0x11, 0x01,
                                          0x03, 0x11, 0x01 };

    basic_array_source<char> input_source(data.data(), data.size());
    stream<basic_array_source<char>> input_stream(input_source);

    Context context;
    SOF0Decoder decoder;

    decoder.Invoke(input_stream, context);

    BOOST_REQUIRE(context.dct.precision == 8);
    BOOST_REQUIRE(context.dct.width == 16);
    BOOST_REQUIRE(context.dct.height == 16);

    BOOST_REQUIRE(context.dct.components.size() == 3);

    BOOST_REQUIRE(context.dct.components[0].id == 1);
    BOOST_REQUIRE(context.dct.components[0].h == 2);
    BOOST_REQUIRE(context.dct.components[0].v == 2);
    BOOST_REQUIRE(context.dct.components[0].dqtId == 0);

    BOOST_REQUIRE(context.dct.components[1].id == 2);
    BOOST_REQUIRE(context.dct.components[1].h == 1);
    BOOST_REQUIRE(context.dct.components[1].v == 1);
    BOOST_REQUIRE(context.dct.components[1].dqtId == 1);

    BOOST_REQUIRE(context.dct.components[2].id == 3);
    BOOST_REQUIRE(context.dct.components[2].h == 1);
    BOOST_REQUIRE(context.dct.components[2].v == 1);
    BOOST_REQUIRE(context.dct.components[2].dqtId == 1);
}

BOOST_AUTO_TEST_SUITE_END()
