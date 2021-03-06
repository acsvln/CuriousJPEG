#include "dqt_decoder.hpp"
#include <boost/test/unit_test.hpp>

#include <array>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/assignment.hpp>

#include "exceptions.hpp"
#include "testing_shared_data.hpp"
#include "testing_utility.hpp"

BOOST_AUTO_TEST_SUITE(DQTDecoderTests)

BOOST_AUTO_TEST_CASE(Invoke_DQT1) {
  const std::array<uint8_t, 67> Source = {
      0x00, 0x43, 0x00, 0xA0, 0x6E, 0x78, 0x8C, 0x78, 0x64, 0xA0, 0x8C, 0x82,
      0x8C, 0xB4, 0xAA, 0xA0, 0xBE, 0xF0, 0xFF, 0xFF, 0xF0, 0xDC, 0xDC, 0xF0,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  const auto Expected = DefineQuantizationTable1();

  DecoderContext Context;
  invokeDecoderWithDataBuffer<DQTDecoder>(Context, Source);

  BOOST_REQUIRE_EQUAL(Context.DQT_Vector.size(), 1);
  BOOST_REQUIRE_EQUAL(Expected, Context.DQT_Vector.at(0));
}

BOOST_AUTO_TEST_CASE(Invoke_DQT2) {
  const std::array<uint8_t, 67> Source = {
      0x00, 0x43, 0x01, 0xAA, 0xB4, 0xB4, 0xF0, 0xD2, 0xF0, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  const auto Expected = DefineQuantizationTable2();

  DecoderContext Context;
  invokeDecoderWithDataBuffer<DQTDecoder>(Context, Source);

  BOOST_REQUIRE_EQUAL(Context.DQT_Vector.size(), 2);
  BOOST_REQUIRE_EQUAL(Expected, Context.DQT_Vector.at(1));
}

BOOST_AUTO_TEST_CASE(UnimplementedTableElementSize) {
  const std::array<uint8_t, 67> Source = {
      0x00, 0x43, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

  DecoderContext Context;
  BOOST_CHECK_THROW(invokeDecoderWithDataBuffer<DQTDecoder>(Context, Source),
                    NotImplementedException);
}

BOOST_AUTO_TEST_CASE(Invoke_BlankWhiteImage_DQT1) {
    const std::array<uint8_t, 0x43> Source = {
        0x00, 0x43, 0x00,
        0x0E, 0x0A, 0x0B, 0x0D, 0x0B, 0x09, 0x0E,
        0x0D, 0x0C, 0x0D, 0x10, 0x0F, 0x0E, 0x11,
        0x16, 0x24, 0x17, 0x16, 0x14, 0x14, 0x16,
        0x2C, 0x20, 0x21, 0x1A, 0x24, 0x34, 0x2E,
        0x37, 0x36, 0x33, 0x2E, 0x32, 0x32, 0x3A,
        0x41, 0x53, 0x46, 0x3A, 0x3D, 0x4E, 0x3E,
        0x32, 0x32, 0x48, 0x62, 0x49, 0x4E, 0x56,
        0x58, 0x5D, 0x5E, 0x5D, 0x38, 0x45, 0x66,
        0x6D, 0x65, 0x5A, 0x6C, 0x53, 0x5B, 0x5D,
        0x59
    };

    DecoderContext Context;
    invokeDecoderWithDataBuffer<DQTDecoder>(Context, Source);

    boost::numeric::ublas::matrix<uint16_t> Expected(8, 8);
    // clang-format off
    Expected <<=
            0x0E, 0x0A, 0x09, 0x0E, 0x16, 0x24, 0x2E, 0x37,
            0x0B, 0x0B, 0x0D, 0x11, 0x17, 0x34, 0x36, 0x32,
            0x0D, 0x0C, 0x0E, 0x16, 0x24, 0x33, 0x3E, 0x32,
            0x0D, 0x0F, 0x14, 0x1A, 0x2E, 0x4E, 0x48, 0x38,
            0x10, 0x14, 0x21, 0x32, 0x3D, 0x62, 0x5D, 0x45,
            0x16, 0x20, 0x32, 0x3A, 0x49, 0x5E, 0x66, 0x53,
            0x2C, 0x3A, 0x46, 0x4E, 0x5D, 0x6D, 0x6C, 0x5B,
            0x41, 0x53, 0x56, 0x58, 0x65, 0x5A, 0x5D, 0x59;
    // clang-format on

    BOOST_REQUIRE_EQUAL(Context.DQT_Vector.size(), 1);
    BOOST_REQUIRE_EQUAL(Expected, Context.DQT_Vector.at(0));
}

BOOST_AUTO_TEST_CASE(Invoke_BlankWhiteImage_DQT2) {
    const std::array<uint8_t, 0x43> Source = {
      0x00, 0x43, 0x01,
      0x0F, 0x10, 0x10, 0x16, 0x13, 0x16, 0x2A, 0x17, 0x17, 0x2A, 0x59, 0x3B, 0x32, 0x3B, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59,
      0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59
    };

    DecoderContext Context;
    invokeDecoderWithDataBuffer<DQTDecoder>(Context, Source);

    boost::numeric::ublas::matrix<uint16_t> Expected(8, 8);
    // clang-format off
    Expected <<=
        0x0F, 0x10, 0x16, 0x2A, 0x59, 0x59, 0x59, 0x59,
        0x10, 0x13, 0x17, 0x3B, 0x59, 0x59, 0x59, 0x59,
        0x16, 0x17, 0x32, 0x59, 0x59, 0x59, 0x59, 0x59,
        0x2A, 0x3B, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59,
        0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59,
        0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59,
        0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59,
        0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59;
    // clang-format on

    BOOST_REQUIRE_EQUAL(Context.DQT_Vector.size(), 2);
    BOOST_REQUIRE_EQUAL(Expected, Context.DQT_Vector.at(1));
}

BOOST_AUTO_TEST_SUITE_END()
