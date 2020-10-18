#include "dht_decoder.hpp"
#include <boost/test/unit_test.hpp>

#include <array>

#include "exceptions.hpp"
#include "testing_utility.hpp"

BOOST_AUTO_TEST_SUITE(DHTDecoderTests)

BOOST_AUTO_TEST_CASE(Invoke_DC_0) {
  const std::array<char, 21> Source = {
      0x00, 0x15, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02};

  DecoderContext Context;
  invokeDecoderWithDataBuffer<DHTDecoder>(Context, Source);
  BOOST_REQUIRE_EQUAL(Context.DC_HuffmanTables.size(), 1);
  BOOST_REQUIRE_EQUAL(Context.AC_HuffmanTables.size(), 0);

  const auto root = Context.DC_HuffmanTables.at(0);
  BOOST_REQUIRE_EQUAL(root->left()->data(), 0x03);
  BOOST_REQUIRE_EQUAL(root->right()->left()->data(), 0x02);
}

BOOST_AUTO_TEST_CASE(Invoke_AC_0) {
  const std::array<char, 26> Source = {0x00, 0x1a, 0x10, 0x01, 0x00, 0x02, 0x03,
                                       0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
                                       0x12, 0x02, 0x11, 0x31, 0x21};

  DecoderContext Context;
  invokeDecoderWithDataBuffer<DHTDecoder>(Context, Source);
  BOOST_REQUIRE_EQUAL(Context.DC_HuffmanTables.size(), 0);
  BOOST_REQUIRE_EQUAL(Context.AC_HuffmanTables.size(), 1);

  const auto root = Context.AC_HuffmanTables.at(0);
  BOOST_REQUIRE_EQUAL(root->left()->data(), 0x01);
  BOOST_REQUIRE_EQUAL(root->right()->left()->left()->data(), 0x00);
  BOOST_REQUIRE_EQUAL(root->right()->left()->right()->data(), 0x12);
  BOOST_REQUIRE_EQUAL(root->right()->right()->left()->left()->data(), 0x02);
  BOOST_REQUIRE_EQUAL(root->right()->right()->left()->right()->data(), 0x11);
  BOOST_REQUIRE_EQUAL(root->right()->right()->right()->left()->data(), 0x31);
  BOOST_REQUIRE_EQUAL(root->right()->right()->right()->right()->left()->data(),
                      0x21);
}

BOOST_AUTO_TEST_CASE(Invoke_DC_1) {
  const std::array<char, 21> Source = {
      0x00, 0x15, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

  DecoderContext Context;
  invokeDecoderWithDataBuffer<DHTDecoder>(Context, Source);
  BOOST_REQUIRE_EQUAL(Context.DC_HuffmanTables.size(), 2);
  BOOST_REQUIRE_EQUAL(Context.AC_HuffmanTables.size(), 0);

  const auto root = Context.DC_HuffmanTables[1];
  BOOST_REQUIRE_EQUAL(root->left()->data(), 0x00);
  BOOST_REQUIRE_EQUAL(root->right()->left()->data(), 0x01);
}

BOOST_AUTO_TEST_CASE(Invoke_AC_1) {
  const std::array<char, 22> Source = {
      0x00, 0x16, 0x11, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x01};

  DecoderContext Context;
  invokeDecoderWithDataBuffer<DHTDecoder>(Context, Source);
  BOOST_REQUIRE_EQUAL(Context.DC_HuffmanTables.size(), 0);
  BOOST_REQUIRE_EQUAL(Context.AC_HuffmanTables.size(), 2);

  const auto root = Context.AC_HuffmanTables[1];
  BOOST_REQUIRE_EQUAL(root->left()->data(), 0x11);
  BOOST_REQUIRE_EQUAL(root->right()->left()->data(), 0x00);
  BOOST_REQUIRE_EQUAL(root->right()->right()->left()->data(), 0x01);
}

BOOST_AUTO_TEST_SUITE_END()
