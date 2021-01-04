#include "bit_extractor.hpp"
#include <boost/test/unit_test.hpp>

#include "testing_utility.hpp"

BOOST_AUTO_TEST_SUITE(BitExtractorTests)

namespace {
namespace ios = boost::iostreams;
} // namespace

BOOST_AUTO_TEST_CASE(getOneBit) {
  // clang-format off
  const std::array<uint8_t, 4> Source = {
      0b10101001, 0b01111011, 0b01100101, 0b00000001
  };
  // clang-format on
  const auto Buffer = charVectorForBuffer(Source);

  ios::basic_array_source<char> InputSource{Buffer.data(), Buffer.size()};
  ios::stream<ios::basic_array_source<char>> InputStream{InputSource};

  BitExtractor Extractor{InputStream};

  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);

  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);

  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);

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
  // clang-format off
  const std::array<std::uint8_t, 4> Source = {
      0b10101001, 0b01111011, 0b01100101, 0b00000001
  };
  // clang-format on

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

BOOST_AUTO_TEST_CASE(getOneBit_XXFF00YY) {
  // clang-format off
  const std::array<std::uint8_t, 4> Source = {
      0b10100001, 0xFF, 0x00, 0b00010010
  };
  // clang-format on

  const auto Buffer = charVectorForBuffer(Source);

  ios::basic_array_source<char> InputSource{Buffer.data(), Buffer.size()};
  ios::stream<ios::basic_array_source<char>> InputStream{InputSource};

  BitExtractor Extractor{InputStream};

  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);

  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);

  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 1);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(), 0);
}

BOOST_AUTO_TEST_CASE(getFewBits_FF00YY) {
  // clang-format off
  const std::array<std::uint8_t, 5> Source = {
      0b10100001, 0b10110001, 0xFF, 0x00, 0b00010010
  };
  // clang-format on

  const auto Buffer = charVectorForBuffer(Source);

  ios::basic_array_source<char> InputSource{Buffer.data(), Buffer.size()};
  ios::stream<ios::basic_array_source<char>> InputStream{InputSource};

  BitExtractor Extractor{InputStream};

  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(4), 0b1010);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(6), 0b000110);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(8), 0b11000111);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(10), 0b1111110001);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(4), 0b0010);
}

BOOST_AUTO_TEST_CASE(getFewBits_00FF00XX) {
  // clang-format off
  const std::array<std::uint8_t, 5> Source = {
      0b10100001, 0x00, 0xFF, 0x00, 0b00010010
  };
  // clang-format on

  const auto Buffer = charVectorForBuffer(Source);

  ios::basic_array_source<char> InputSource{Buffer.data(), Buffer.size()};
  ios::stream<ios::basic_array_source<char>> InputStream{InputSource};

  BitExtractor Extractor{InputStream};

  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(4), 0b1010);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(6), 0b000100);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(8), 0b00000011);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(10), 0b1111110001);
  BOOST_REQUIRE_EQUAL(Extractor.nextNumber(4), 0b0010);
}

BOOST_AUTO_TEST_SUITE_END()
