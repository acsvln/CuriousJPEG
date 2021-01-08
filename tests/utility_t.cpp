#include "utility.hpp"
#include <boost/test/unit_test.hpp>

#include <array>
#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "testing_shared_data.hpp"
#include "testing_utility.hpp"

BOOST_AUTO_TEST_SUITE(UtilityTests)

BOOST_AUTO_TEST_CASE(LowHighByte) {
  const uint8_t Source = 0xFA;

  const auto ResultLow = highByte(Source);
  BOOST_CHECK_EQUAL(ResultLow, 0xF);

  const auto ResultHigh = lowByte(Source);
  BOOST_CHECK_EQUAL(ResultHigh, 0xA);
}

BOOST_AUTO_TEST_CASE(CreateZigZagMatrix_DQT) {
  const std::array<uint16_t, 64> Source = {
      0xA0, 0x6E, 0x78, 0x8c, 0x78, 0x64, 0xA0, 0x8c, 0x82, 0x8c, 0xb4,
      0xaa, 0xa0, 0xbe, 0xf0, 0xFF, 0xFF, 0xF0, 0xDC, 0xDC, 0xF0, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  const auto Expected = DefineQuantizationTable1();
  const auto Result = CreateZigZagMatrix(Source);
  BOOST_REQUIRE_EQUAL(Expected, Result);
}

BOOST_AUTO_TEST_CASE(CreateZigZagMatrix_Sequence) {
  const std::array<uint16_t, 64> Source = {
      1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
      17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
      33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
      49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};

  boost::numeric::ublas::matrix<uint16_t> Expected(8, 8);
  // clang-format off
  Expected <<=    1,   2,  6,  7, 15, 16, 28, 29,
                  3,   5,  8, 14, 17, 27, 30, 43,
                  4,   9, 13, 18, 26, 31, 42, 44,
                  10, 12, 19, 25, 32, 41, 45, 54,
                  11, 20, 24, 33, 40, 46, 53, 55,
                  21, 23, 34, 39, 47, 52, 56, 61,
                  22, 35, 38, 48, 51, 57, 60, 62,
                  36, 37, 49, 50, 58, 59, 63, 64;
  // clang-format on

  const auto Result = CreateZigZagMatrix(Source);
  BOOST_REQUIRE_EQUAL(Expected, Result);
}

BOOST_AUTO_TEST_SUITE_END()
