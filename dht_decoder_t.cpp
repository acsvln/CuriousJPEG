#include <boost/test/unit_test.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <fstream>
#include <iostream>

using namespace boost::iostreams;
using namespace std;

#include "dht_decoder.hpp"

BOOST_AUTO_TEST_SUITE(DHTDecoderTests)


BOOST_AUTO_TEST_CASE(Invoke) {
  const std::array<char, 21> data_DC_0 = {
      0x00, 0x15, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02};
  const std::array<char, 26> data_AC_0 = {
      0x00, 0x1a, 0x10, 0x01, 0x00, 0x02, 0x03, 0x01, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x00, 0x12, 0x02, 0x11, 0x31, 0x21};
  const std::array<char, 21> data_DC_1 = {
      0x00, 0x15, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
  const std::array<char, 22> data_AC_1 = {
      0x00, 0x16, 0x11, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x01};

  Context context;

  const auto invokeDecoder = [&context](const auto &data) {
    DHTDecoder decoder;
    basic_array_source<char> input_source(data.data(), data.size());
    stream<basic_array_source<char>> input_stream(input_source);
    decoder.Invoke(input_stream, context);
  };

  invokeDecoder(data_DC_0);
  invokeDecoder(data_DC_1);
  invokeDecoder(data_AC_1);
  invokeDecoder(data_AC_0);

  BOOST_REQUIRE_EQUAL(context.DC_HuffmanTables.size(), 2);
  BOOST_REQUIRE_EQUAL(context.AC_HuffmanTables.size() , 2);

  const auto root_DC_0 = context.DC_HuffmanTables[0];
  BOOST_REQUIRE_EQUAL(root_DC_0->left()->data(), 0x03);
  BOOST_REQUIRE_EQUAL(root_DC_0->right()->left()->data(), 0x02);

  const auto root_DC_1 = context.DC_HuffmanTables[1];
  BOOST_REQUIRE_EQUAL(root_DC_1->left()->data(), 0x00);
  BOOST_REQUIRE_EQUAL(root_DC_1->right()->left()->data(),0x01);

  const auto root_AC_0 = context.AC_HuffmanTables[0];
  BOOST_REQUIRE_EQUAL(root_AC_0->left()->data(), 0x01);
  BOOST_REQUIRE_EQUAL(root_AC_0->right()->left()->left()->data(), 0x00);
  BOOST_REQUIRE_EQUAL(root_AC_0->right()->left()->right()->data(), 0x12);
  BOOST_REQUIRE_EQUAL(root_AC_0->right()->right()->left()->left()->data(), 0x02);
  BOOST_REQUIRE_EQUAL(root_AC_0->right()->right()->left()->right()->data(), 0x11);
  BOOST_REQUIRE_EQUAL(root_AC_0->right()->right()->right()->left()->data(), 0x31);
  BOOST_REQUIRE_EQUAL(root_AC_0->right()->right()->right()->right()->left()->data(), 0x21);

  const auto root_AC_1 = context.AC_HuffmanTables[1];
  BOOST_REQUIRE_EQUAL(root_AC_1->left()->data(),0x11);
  BOOST_REQUIRE_EQUAL(root_AC_1->right()->left()->data(), 0x00);
  BOOST_REQUIRE_EQUAL(root_AC_1->right()->right()->left()->data(), 0x01);
}

BOOST_AUTO_TEST_SUITE_END()
