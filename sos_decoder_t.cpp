#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <fstream>
#include <iostream>
#include <array>

using namespace boost::iostreams;
using namespace std;

#include "sos_decoder.hpp"

BOOST_AUTO_TEST_SUITE(SOSDecoderTests)

BOOST_AUTO_TEST_CASE(Invoke) {
    const std::array<char, 31> data = {
        (char)0x00, (char)0x0c, (char)0x03, (char)0x01, (char)0x00, (char)0x02,
        (char)0x11, (char)0x03, (char)0x11, (char)0x00, (char)0x3f, (char)0x00,
        (char)0xae, (char)0xe7, (char)0x61, (char)0xf2, (char)0x1b, (char)0xd5,
        (char)0x22, (char)0x85, (char)0x5d, (char)0x04, (char)0x3c, (char)0x82,
        (char)0xc8, (char)0x48, (char)0xb1, (char)0xdc, (char)0xbf, (char)0xff,
        (char)0xd9
     };

    basic_array_source<char> input_source(data.data(), data.size());
    stream<basic_array_source<char>> input_stream(input_source);

    Context context;
    SOSDecoder decoder;

    decoder.Invoke(input_stream, context);
}

BOOST_AUTO_TEST_SUITE_END()
