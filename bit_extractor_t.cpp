#include "bit_extractor.hpp"
#include <boost/test/unit_test.hpp>

#include "testing_utility.hpp"

BOOST_AUTO_TEST_SUITE(BitExtractorTests)

namespace {
namespace ios = boost::iostreams;
} // namespace

BOOST_AUTO_TEST_CASE(getOneBit) {
  const std::array<uint8_t, 4> Source = {0b10101001, 0b01111011, 0b01100101,
                                         0b00000001};
  const auto Buffer = charVectorForBuffer(Source);

  ios::basic_array_source<char> InputSource{Buffer.data(), Buffer.size()};
  ios::stream<ios::basic_array_source<char>> InputStream{InputSource};

  BitExtractor Extractor{InputStream};

  // 0b10101001
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);

  // 0b01111011
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);

  // 0b01100101
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);

  // 0b00000001
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
}

BOOST_AUTO_TEST_CASE(getFewBits) {
  const std::array<std::uint8_t, 4> Source = {0b10101001, 0b01111011,
                                              0b01100101, 0b00000001};

  const auto Buffer = charVectorForBuffer(Source);

  ios::basic_array_source<char> InputSource{Buffer.data(), Buffer.size()};
  ios::stream<ios::basic_array_source<char>> InputStream{InputSource};

  BitExtractor Extractor{InputStream};

  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(6), 0b101010);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(7), 0b0101111);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(8), 0b01101100);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(5), 0b10100);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(2), 0b00);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(4), 0b0001);
}

BOOST_AUTO_TEST_CASE(getOneBit_x) {
  const std::array<std::uint8_t, 2> Source = {
      0b10100100, 0b10000000
  };

  const auto Buffer = charVectorForBuffer(Source);

  ios::basic_array_source<char> InputSource{Buffer.data(), Buffer.size()};
  ios::stream<ios::basic_array_source<char>> InputStream{InputSource};

  BitExtractor Extractor{InputStream};

  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);

  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
}

BOOST_AUTO_TEST_CASE(getOneBit_y) {
    const std::array<std::uint8_t, 2> Source = { 0xD8, 0x85 };

    const auto Buffer = charVectorForBuffer(Source);

    ios::basic_array_source<char> InputSource{Buffer.data(), Buffer.size()};
    ios::stream<ios::basic_array_source<char>> InputStream{InputSource};

    BitExtractor Extractor{InputStream};

    // 1101100010000101

    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);

    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
    BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
}

BOOST_AUTO_TEST_SUITE_END()
