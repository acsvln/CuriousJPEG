#include <boost/test/unit_test.hpp>

#include <array>

#include "sof0_decoder.hpp"
#include "testing_utility.hpp"

BOOST_AUTO_TEST_SUITE(SOF0DecoderTests)

BOOST_AUTO_TEST_CASE(Invoke) {
  std::array<char, 17> const Source{0x00, 0x11, 0x08, 0x00, 0x10, 0x00,
                                    0x10, 0x03, 0x01, 0x22, 0x00, 0x02,
                                    0x11, 0x01, 0x03, 0x11, 0x01};

  Context Ctx;
  invokeDecoderWithDataBuffer<SOF0Decoder>(Ctx, Source);

  BOOST_CHECK_EQUAL(Ctx.dct.precision, 8);
  BOOST_CHECK_EQUAL(Ctx.dct.width, 16u);
  BOOST_CHECK_EQUAL(Ctx.dct.height, 16u);
  BOOST_CHECK_EQUAL(Ctx.dct.maxH, 2u);
  BOOST_CHECK_EQUAL(Ctx.dct.maxV, 2u);

  BOOST_CHECK_EQUAL(Ctx.dct.components.size(), 3u);

  BOOST_CHECK_EQUAL(Ctx.dct.components[0].id, 1);
  BOOST_CHECK_EQUAL(Ctx.dct.components[0].h, 2);
  BOOST_CHECK_EQUAL(Ctx.dct.components[0].v, 2);
  BOOST_CHECK_EQUAL(Ctx.dct.components[0].dqtId, 0);

  BOOST_CHECK_EQUAL(Ctx.dct.components[1].id, 2);
  BOOST_CHECK_EQUAL(Ctx.dct.components[1].h, 1);
  BOOST_CHECK_EQUAL(Ctx.dct.components[1].v, 1);
  BOOST_CHECK_EQUAL(Ctx.dct.components[1].dqtId, 1);

  BOOST_CHECK_EQUAL(Ctx.dct.components[2].id, 3);
  BOOST_CHECK_EQUAL(Ctx.dct.components[2].h, 1);
  BOOST_CHECK_EQUAL(Ctx.dct.components[2].v, 1);
  BOOST_CHECK_EQUAL(Ctx.dct.components[2].dqtId, 1);
}

BOOST_AUTO_TEST_SUITE_END()
