#include "sof0_decoder.hpp"
#include <boost/test/unit_test.hpp>

#include <array>

#include "testing_utility.hpp"

BOOST_AUTO_TEST_SUITE(SOF0DecoderTests)

BOOST_AUTO_TEST_CASE(Invoke) {
  std::array<char, 17> const Source = {0x00, 0x11, 0x08, 0x00, 0x10, 0x00,
                                       0x10, 0x03, 0x01, 0x22, 0x00, 0x02,
                                       0x11, 0x01, 0x03, 0x11, 0x01};

  Context Ctx;
  invokeDecoderWithDataBuffer<SOF0Decoder>(Ctx, Source);

  BOOST_CHECK_EQUAL(Ctx.dct.Precision, 8);
  BOOST_CHECK_EQUAL(Ctx.dct.Width, 16u);
  BOOST_CHECK_EQUAL(Ctx.dct.Height, 16u);
  BOOST_CHECK_EQUAL(Ctx.dct.MaxH, 2u);
  BOOST_CHECK_EQUAL(Ctx.dct.MaxV, 2u);

  BOOST_REQUIRE_EQUAL(Ctx.dct.Components.size(), 3);

  BOOST_CHECK_EQUAL(Ctx.dct.Components[0].Id, 1);
  BOOST_CHECK_EQUAL(Ctx.dct.Components[0].H, 2);
  BOOST_CHECK_EQUAL(Ctx.dct.Components[0].V, 2);
  BOOST_CHECK_EQUAL(Ctx.dct.Components[0].DQT_Id, 0);

  BOOST_CHECK_EQUAL(Ctx.dct.Components[1].Id, 2);
  BOOST_CHECK_EQUAL(Ctx.dct.Components[1].H, 1);
  BOOST_CHECK_EQUAL(Ctx.dct.Components[1].V, 1);
  BOOST_CHECK_EQUAL(Ctx.dct.Components[1].DQT_Id, 1);

  BOOST_CHECK_EQUAL(Ctx.dct.Components[2].Id, 3);
  BOOST_CHECK_EQUAL(Ctx.dct.Components[2].H, 1);
  BOOST_CHECK_EQUAL(Ctx.dct.Components[2].V, 1);
  BOOST_CHECK_EQUAL(Ctx.dct.Components[2].DQT_Id, 1);
}

BOOST_AUTO_TEST_SUITE_END()
