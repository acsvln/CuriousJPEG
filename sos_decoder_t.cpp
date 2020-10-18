#include "sos_decoder.hpp"
#include <boost/test/unit_test.hpp>

#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "exceptions.hpp"
#include "testing_shared_data.hpp"
#include "testing_utility.hpp"

//-------------------------------------
BOOST_AUTO_TEST_SUITE(SOSDecoderTests)

namespace {

namespace ios = boost::iostreams;

class TestedDecoder final : public SOSDecoder {
public:
  ~TestedDecoder() final = default;

  using SOSDecoder::Channel;
  using SOSDecoder::DataUnit;
  using SOSDecoder::MinimumCodedUnit;

  using SOSDecoder::convertYCbCrToRGB;
  using SOSDecoder::locateNodeInHuffmanTree;
  using SOSDecoder::normalizeReversedDQT;
  using SOSDecoder::quantMCU;
  using SOSDecoder::readDU;
  using SOSDecoder::readMCU;
  using SOSDecoder::reverseDQT;
};

//-------------------------------------
auto DC_Tree_0() {
  // clang-format off
  return HuffmanTree::Builder{}
        .left( 0x03 )
        .right().left( 0x02 )
        .end()
    .done();
  // clang-format on
}

auto DC_Tree_1() {
  // clang-format off
  return HuffmanTree::Builder{}
        .left( 0x00 )
        .right().left( 0x01 )
        .end()
    .done();
  // clang-format on
}

auto AC_Tree_0() {
  // clang-format off
  return HuffmanTree::Builder{}
    .left( 0x01 )
    .right()
        .left().left( 0x00 )
               .right( 0x12 )
               .end()
        .right()
            .left()
                .left(0x02)
                .right(0x11)
                .end()
            .right()
                .left(0x31)
                .right()
                    .left(0x21)
                    .end()
                .end()
            .end()
        .end()
    .done();
  // clang-format on
}

auto AC_Tree_1() {
  // clang-format off
  return HuffmanTree::Builder{}
        .left( 0x11 )
        .right()
            .left( 0x00 )
            .right().left( 0x01 )
            .end()
        .end()
    .done();
  // clang-format on
}

auto Y1_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    2,  0,  3, 0, 0, 0, 0, 0,
    0,  1,  2, 0, 0, 0, 0, 0,
    0, -1, -1, 0, 0, 0, 0, 0,
    1,  0,  0, 0, 0, 0, 0, 0,
    0,  0,  0, 0, 0, 0, 0, 0,
    0,  0,  0, 0, 0, 0, 0, 0,
    0,  0,  0, 0, 0, 0, 0, 0,
    0,  0,  0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Y2_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    -4,  1, 1, 1, 0, 0, 0, 0,
     0,  0, 1, 0, 0, 0, 0, 0,
     0, -1, 0, 0, 0, 0, 0, 0,
     0,  0, 0, 0, 0, 0, 0, 0,
     0,  0, 0, 0, 0, 0, 0, 0,
     0,  0, 0, 0, 0, 0, 0, 0,
     0,  0, 0, 0, 0, 0, 0, 0,
     0,  0, 0, 0, 0, 0, 0, 0;
  // clang-format on

  return Table;
}

auto Y3_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
     5, -1,  1, 0, 0, 0, 0, 0,
    -1, -2, -1, 0, 0, 0, 0, 0,
     0, -1,  0, 0, 0, 0, 0, 0,
    -1,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Y4_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    -4,  2,  2, 1, 0, 0, 0, 0,
    -1,  0, -1, 0, 0, 0, 0, 0,
    -1, -1,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Cb_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
       -1, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Cr_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    0,  0, 0, 0, 0, 0, 0, 0,
    1, -1, 0, 0, 0, 0, 0, 0,
    1,  0, 0, 0, 0, 0, 0, 0,
    0,  0, 0, 0, 0, 0, 0, 0,
    0,  0, 0, 0, 0, 0, 0, 0,
    0,  0, 0, 0, 0, 0, 0, 0,
    0,  0, 0, 0, 0, 0, 0, 0,
    0,  0, 0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Cs1_1_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    2,  0,  3, 0, 0, 0, 0, 0,
    0,  1,  2, 0, 0, 0, 0, 0,
    0, -1, -1, 0, 0, 0, 0, 0,
    1,  0,  0, 0, 0, 0, 0, 0,
    0,  0,  0, 0, 0, 0, 0, 0,
    0,  0,  0, 0, 0, 0, 0, 0,
    0,  0,  0, 0, 0, 0, 0, 0,
    0,  0,  0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Cs1_2_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    -2,  1, 1, 1, 0, 0, 0, 0,
     0,  0, 1, 0, 0, 0, 0, 0,
     0, -1, 0, 0, 0, 0, 0, 0,
     0,  0, 0, 0, 0, 0, 0, 0,
     0,  0, 0, 0, 0, 0, 0, 0,
     0,  0, 0, 0, 0, 0, 0, 0,
     0,  0, 0, 0, 0, 0, 0, 0,
     0,  0, 0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Cs1_3_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
     3, -1,  1, 0, 0, 0, 0, 0,
    -1, -2, -1, 0, 0, 0, 0, 0,
     0, -1,  0, 0, 0, 0, 0, 0,
    -1,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Cs1_4_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    -1,  2,  2, 1, 0, 0, 0, 0,
    -1,  0, -1, 0, 0, 0, 0, 0,
    -1, -1,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0,
     0,  0,  0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Quanted_Cs1_1_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    320,    0,  300, 0, 0, 0, 0, 0,
      0,  120,  280, 0, 0, 0, 0, 0,
      0, -130, -160, 0, 0, 0, 0, 0,
    140,    0,    0, 0, 0, 0, 0, 0,
      0,    0,    0, 0, 0, 0, 0, 0,
      0,    0,    0, 0, 0, 0, 0, 0,
      0,    0,    0, 0, 0, 0, 0, 0,
      0,    0,    0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Quanted_Cs1_2_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    -320,  110, 100, 160, 0, 0, 0, 0,
       0,    0, 140,   0, 0, 0, 0, 0,
       0, -130,   0,   0, 0, 0, 0, 0,
       0,    0,   0,   0, 0, 0, 0, 0,
       0,    0,   0,   0, 0, 0, 0, 0,
       0,    0,   0,   0, 0, 0, 0, 0,
       0,    0,   0,   0, 0, 0, 0, 0,
       0,    0,   0,   0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Quanted_Cs1_3_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    480, -110,  100, 0, 0, 0, 0, 0,
   -120, -240, -140, 0, 0, 0, 0, 0,
      0, -130,    0, 0, 0, 0, 0, 0,
   -140,    0,    0, 0, 0, 0, 0, 0,
      0,    0,    0, 0, 0, 0, 0, 0,
      0,    0,    0, 0, 0, 0, 0, 0,
      0,    0,    0, 0, 0, 0, 0, 0,
      0,    0,    0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Quanted_Cs1_4_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    -160,  220,  200, 160, 0, 0, 0, 0,
    -120,    0, -140,   0, 0, 0, 0, 0,
    -140, -130,    0,   0, 0, 0, 0, 0,
       0,    0,    0,   0, 0, 0, 0, 0,
       0,    0,    0,   0, 0, 0, 0, 0,
       0,    0,    0,   0, 0, 0, 0, 0,
       0,    0,    0,   0, 0, 0, 0, 0,
       0,    0,    0,   0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Quanted_Cb_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    -170,   0, 0, 0, 0, 0, 0, 0,
     180, 210, 0, 0, 0, 0, 0, 0,
       0,   0, 0, 0, 0, 0, 0, 0,
       0,   0, 0, 0, 0, 0, 0, 0,
       0,   0, 0, 0, 0, 0, 0, 0,
       0,   0, 0, 0, 0, 0, 0, 0,
       0,   0, 0, 0, 0, 0, 0, 0,
       0,   0, 0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Quanted_Cr_Table() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
      0,    0, 0, 0, 0, 0, 0, 0,
    180, -210, 0, 0, 0, 0, 0, 0,
    240,    0, 0, 0, 0, 0, 0, 0,
      0,    0, 0, 0, 0, 0, 0, 0,
      0,    0, 0, 0, 0, 0, 0, 0,
      0,    0, 0, 0, 0, 0, 0, 0,
      0,    0, 0, 0, 0, 0, 0, 0,
      0,    0, 0, 0, 0, 0, 0, 0;
  // clang-format on
  return Table;
}

auto Reversed_Y1() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    138,  92, 27, -17, -17,  28, 93, 139,
    136,  82,  5, -51, -55,  -8, 61, 111,
    143,  80, -9, -77, -89, -41, 32,  86,
    157,  95,  6, -62, -76, -33, 36,  86,
    147, 103, 37, -12, -21,  11, 62, 100,
     87,  72, 50,  36,  37,  55, 79,  95,
    -10,   5, 31,  56,  71,  73, 68,  62,
    -87, -50,  6,  56,  79,  72, 48,  29;
  // clang-format on
  return Table;
}

auto Reversed_Y2() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
     21, -34, -93, -105, -70, -26,  -5,   -5,
     33, -21, -81,  -97, -68, -34, -22,  -27,
     49,  -4, -64,  -83, -64, -44, -46,  -60,
     54,   4, -49,  -68, -56, -49, -66,  -89,
     42,   0, -44,  -56, -43, -43, -71, -102,
     13, -19, -49,  -47, -28, -29, -61,  -96,
    -20, -44, -59,  -43, -15, -11, -44,  -80,
    -42, -60, -67,  -42,  -6,   0, -32,  -68;
  // clang-format on
  return Table;
}

auto Reversed_Y3() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    -103, -77, -35, 12, 55, 85, 102, 109,
     -43, -25,   6, 41, 73, 96, 110, 116,
      31,  38,  50, 66, 81, 95, 105, 110,
      78,  72,  64, 59, 59, 66,  74,  80,
      94,  78,  53, 32, 23, 28,  41,  51,
     107,  83,  47, 18,  8, 18,  39,  55,
     133, 105,  64, 33, 26, 44,  75,  98,
     158, 129,  86, 55, 51, 76, 113, 140;
  // clang-format on
  return Table;
}

auto Reversed_Y4() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
    -30, -61, -86,  -78, -50, -41, -65, -94,
      8, -28, -62,  -63, -42, -36, -61, -90,
     66,  20, -27,  -42, -31, -29, -53, -81,
    116,  61,  -1,  -29, -25, -22, -42, -66,
    137,  74,   1,  -34, -29, -19, -28, -45,
    125,  59, -18,  -55, -43, -20, -14, -21,
     97,  29, -49,  -81, -60, -23,  -3,  -1,
     74,   6, -71, -100, -72, -25,   2,  10;
  // clang-format on
  return Table;
}

auto Reversed_Cb() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
      60,  52,  38,  20,   0, -18, -32, -40,
      48,  41,  29,  13,  -3, -19, -31, -37,
      25,  20,  12,   2,  -9, -19, -27, -32,
      -4,  -6,  -9, -13, -17, -20, -23, -25,
     -37, -35, -33, -29, -25, -21, -18, -17,
     -67, -63, -55, -44, -33, -22, -14, -10,
     -90, -84, -71, -56, -39, -23, -11,  -4,
    -102, -95, -81, -62, -42, -23,  -9,  -1;
  // clang-format on
  return Table;
}

auto Reversed_Cr() {
  TestedDecoder::DataUnit Table(8, 8);
  // clang-format off
  Table <<=
     19,  27,  41,  60,  80,  99, 113, 120,
      0,   6,  18,  34,  51,  66,  78,  85,
    -27, -22, -14,  -4,   7,  17,  25,  30,
    -43, -41, -38, -34, -30, -27, -24, -22,
    -35, -36, -39, -43, -47, -51, -53, -55,
     -5,  -9, -17, -28, -39, -50, -58, -62,
     32,  26,  14,  -1, -18, -34, -46, -53,
     58,  50,  36,  18,  -2, -20, -34, -42;
  // clang-format on
  return Table;
}

auto ComponentsForDCT() {
  std::vector<DCTComponent> Components;

  Components.resize(3);

  Components[0].Id = 1;
  Components[0].H = 2;
  Components[0].V = 2;
  Components[0].DQT_Id = 0;

  Components[1].Id = 2;
  Components[1].H = 1;
  Components[1].V = 1;
  Components[1].DQT_Id = 1;

  Components[2].Id = 3;
  Components[2].H = 1;
  Components[2].V = 1;
  Components[2].DQT_Id = 1;

  return Components;
}

auto DCT_Table() {
  DCTTable DCT;

  DCT.Precision = 8;
  DCT.Width = 16;
  DCT.Height = 16;
  DCT.Components = ComponentsForDCT();

  return DCT;
}

auto RedChannel() {
  boost::numeric::ublas::matrix<uint8_t> R(16, 16);
  // clang-format off
    R <<=
        255, 247, 193, 149, 168, 213, 255, 255, 255, 206, 174, 162, 216, 255, 255, 255,
        255, 237, 171, 115, 130, 177, 255, 255, 255, 219, 186, 170, 218, 252, 255, 255,
        255, 208, 127,  59,  64, 112, 208, 255, 249, 196, 157, 138, 173, 193, 201, 187,
        255, 223, 142,  74,  77, 120, 212, 255, 254, 204, 172, 153, 181, 188, 181, 158,
        217, 193, 134,  85,  87, 119, 184, 222, 180, 138, 108,  96, 120, 120,  99,  68,
        177, 162, 147, 133, 145, 163, 201, 217, 151, 119, 103, 105, 135, 134, 109,  74,
         58,  73, 102, 127, 146, 148, 148, 142,  66,  42,  31,  47,  79,  83,  53,  17,
          0,  18,  77, 127, 154, 147, 128, 109,  44,  26,  23,  48,  88,  94,  65,  29,
          0,   2,  43,  90, 128, 158, 170, 177,  32,   1,   0,   0,   4,  13,   0,   0,
        36,   54,  84, 119, 146, 169, 178, 184,  70,  34,   0,   0,  12,  18,   0,   0,
        152, 159, 165, 181, 185, 199, 194, 199, 139,  93,  31,  16,  16,  18,   0,   0,
        199, 193, 179, 174, 163, 170, 163, 169, 189, 134,  57,  29,  22,  25,   0,   0,
        255, 251, 217, 196, 171, 176, 168, 178, 230, 177,  81,  46,  35,  45,  26,   9,
        255, 255, 211, 182, 156, 166, 166, 182, 228, 162,  62,  25,  21,  44,  40,  33,
        255, 255, 255, 231, 204, 222, 228, 251, 222, 154,  51,  19,  20,  57,  66,  68,
        255, 255, 255, 253, 229, 254, 255, 255, 199, 131,  29,   0,   8,  55,  71,  79;
  // clang-format on
  return R;
}

auto GreenChannel() {
  boost::numeric::ublas::matrix<uint8_t> G(16, 16);
  // clang-format off
    G <<=
        221, 186, 118,  74,  69, 114, 171, 205,  92,  37,   0,   0,   0,  32,  51,  51,
        221, 176,  96,  40,  31,  78, 139, 189, 104,  50,   0,   0,   0,  24,  34,  29,
        238, 191, 101,  33,  16,  64, 131, 185, 142,  89,  23,   4,  19,  39,  34,  20,
        238, 206, 116,  48,  29,  72, 135, 185, 147,  97,  38,  19,  27,  34,  14,   0,
        255, 242, 174, 125, 113, 145, 192, 230, 168, 126,  78,  66,  76,  76,  47,  16,
        226, 211, 187, 173, 171, 189, 209, 225, 139, 107,  73,  75,  91,  90,  57,  22,
        150, 165, 190, 215, 229, 231, 225, 219, 135, 111,  95, 111, 138, 142, 108,  72,
         73, 110, 165, 215, 237, 230, 205, 186, 113,  95,  87, 112, 147, 153, 120,  84,
         63,  89, 131, 178, 222, 252, 255, 255, 140, 109,  86,  94, 122, 131, 108,  79,
        123, 141, 172, 207, 240, 255, 255, 255, 178, 142, 110, 109, 130, 136, 112,  83,
        186, 193, 206, 222, 240, 254, 255, 255, 233, 187, 144, 129, 143, 145, 123,  95,
        233, 227, 220, 215, 218, 225, 237, 243, 255, 228, 170, 142, 149, 152, 134, 110,
        230, 214, 191, 170, 165, 170, 189, 199, 255, 228, 161, 126, 136, 146, 139, 122,
        243, 219, 185, 156, 150, 160, 187, 203, 255, 213, 142, 105, 122, 145, 153, 146,
        249, 227, 189, 158, 156, 174, 211, 234, 241, 173, 101,  69,  95, 132, 155, 157,
        249, 249, 211, 180, 181, 206, 249, 255, 218, 150,  79,  50,  83, 130, 160, 168;
  // clang-format on
  return G;
}

auto BlueChannel() {
  boost::numeric::ublas::matrix<uint8_t> B(16, 16);
  // clang-format off
    B <<=
        255, 255, 247, 203, 178, 223, 255, 255, 149,  94,   3,   0,  1, 45,  52,  52,
        255, 255, 225, 169, 140, 187, 224, 255, 161, 107,  15,   0,  3, 37,  35,  30,
        255, 255, 192, 124,  90, 138, 183, 237, 172, 119,  30,  11,  9, 29,  16,   2,
        255, 255, 207, 139, 103, 146, 187, 237, 177, 127,  45,  26, 17, 24,   0,   0,
        255, 255, 200, 151, 128, 160, 194, 232, 154, 112,  50,  38, 37, 37,   0,   0,
        255, 244, 213, 199, 186, 204, 211, 227, 125,  93,  45,  47, 52, 51,  10,   0,
        111, 126, 148, 173, 183, 185, 173, 167,  78,  54,  34,  50, 72, 76,  40,   4,
         34,  71, 123, 173, 191, 184, 153, 134,  56,  38,  26,  51, 81, 87,  52,  16,
          0,   0,  31,  78, 125, 155, 179, 186,  54,  23,   5,  13, 46, 55,  33,   4,
         19,  37,  72, 107, 143, 166, 187, 193,  92,  56,  29,  28, 54, 60,  37,   8,
         40,  47,  66,  82, 112, 126, 155, 160, 136,  90,  62,  47, 72, 74,  57,  29,
         87,  81,  80,  75,  90,  97, 124, 130, 186, 131,  88,  60, 78, 81,  68,  44,
         63,  47,  32,  11,  25,  30,  70,  80, 186, 133,  88,  53, 80, 90,  93,  76,
         76,  52,  26,   0,  10,  20,  68,  84, 184, 118,  69,  32, 66, 89, 107, 100,
         74,  52,  24,   0,  10,  28,  93, 116, 151,  83,  38,   6, 52, 89, 123, 125,
         74,  74,  46,  15,  35,  60, 131, 145, 128,  60,  16,   0, 40, 87, 128, 136;

  // clang-format on
  return B;
}

auto ChannelsForMCU() {
  std::vector<TestedDecoder::Channel> Channels;

  Channels.resize(3);

  Channels[0].Id = 1;
  Channels[0].AC_Id = 0;
  Channels[0].DC_Id = 0;

  Channels[1].Id = 2;
  Channels[1].AC_Id = 1;
  Channels[1].DC_Id = 1;

  Channels[2].Id = 3;
  Channels[2].AC_Id = 1;
  Channels[2].DC_Id = 1;

  return Channels;
}

void testLocateNodeInHuffmanTree(
    const std::shared_ptr<HuffmanTree::Node> &Root,
    const std::array<uint8_t, 1> &Source,
    const std::shared_ptr<HuffmanTree::Node> &Expected) {
  const auto Buffer = charVectorForBuffer(Source);
  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);

  BitExtractor Extractor{InputStream};
  const auto Located = TestedDecoder::locateNodeInHuffmanTree(Extractor, Root);
  BOOST_REQUIRE_EQUAL(Located, Expected);
}

void testLocateNodeInHuffmanTreeWithException(
    const std::shared_ptr<HuffmanTree::Node> &Root,
    const std::array<uint8_t, 1> &Source) {
  const auto Buffer = charVectorForBuffer(Source);
  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);

  BitExtractor Extractor{InputStream};
  BOOST_CHECK_THROW(TestedDecoder::locateNodeInHuffmanTree(Extractor, Root),
                    InvalidJPEGDataException);
}

} // namespace

BOOST_AUTO_TEST_CASE(locateNodeInHuffmanTree_DC_0) {
  const auto Root = DC_Tree_0();
  testLocateNodeInHuffmanTree(Root, {0b10000000}, Root->right()->left());
}

BOOST_AUTO_TEST_CASE(locateNodeInHuffmanTree_AC_0) {
  const auto Root = AC_Tree_0();

  testLocateNodeInHuffmanTree(Root, {0b11100000},
                              Root->right()->right()->right()->left());
  testLocateNodeInHuffmanTree(Root, {0b11000000},
                              Root->right()->right()->left()->left());
  testLocateNodeInHuffmanTree(Root, {0b10100000},
                              Root->right()->left()->right());
  testLocateNodeInHuffmanTree(Root, {0b00000000}, Root->left());
  testLocateNodeInHuffmanTree(Root, {0b11110000},
                              Root->right()->right()->right()->right()->left());
  testLocateNodeInHuffmanTree(Root, {0b10000000},
                              Root->right()->left()->left());
}

BOOST_AUTO_TEST_CASE(locateNodeInHuffmanTree_DC_1) {
  const auto Root = DC_Tree_1();
  testLocateNodeInHuffmanTree(Root, {0b10000000}, Root->right()->left());
}

BOOST_AUTO_TEST_CASE(locateNodeInHuffmanTree_AC_1) {
  const auto Root = AC_Tree_1();

  testLocateNodeInHuffmanTree(Root, {0b00000000}, Root->left());
  testLocateNodeInHuffmanTree(Root, {0b10000000}, Root->right()->left());
  testLocateNodeInHuffmanTree(Root, {0b11000000},
                              Root->right()->right()->left());
}

BOOST_AUTO_TEST_CASE(locateNodeInHuffmanTree_ThrowException) {
  const auto Root_DC0 = DC_Tree_0();
  const auto Root_AC0 = AC_Tree_0();
  const auto Root_DC1 = DC_Tree_1();
  const auto Root_AC1 = AC_Tree_1();

  testLocateNodeInHuffmanTreeWithException(Root_DC0, {0b11000000});
  testLocateNodeInHuffmanTreeWithException(Root_AC0, {0b11111000});
  testLocateNodeInHuffmanTreeWithException(Root_DC1, {0b11111000});
  testLocateNodeInHuffmanTreeWithException(Root_AC1, {0b11100000});
}

namespace {

template <class Container>
void testReadDU(const std::shared_ptr<HuffmanTree::Node> &DC_Root,
                const std::shared_ptr<HuffmanTree::Node> &AC_Root,
                const Container &Source,
                const TestedDecoder::DataUnit &Expected) {
  const auto Buffer = charVectorForBuffer(Source);
  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
  BitExtractor Extractor{InputStream};
  const auto DU = TestedDecoder::readDU(Extractor, DC_Root, AC_Root);
  BOOST_REQUIRE_EQUAL(DU, Expected);
}

template <class Container>
void testReadDUWithException(const std::shared_ptr<HuffmanTree::Node> &DC_Root,
                             const std::shared_ptr<HuffmanTree::Node> &AC_Root,
                             const Container &Source) {
  const auto Buffer = charVectorForBuffer(Source);
  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
  BitExtractor Extractor{InputStream};
  BOOST_CHECK_THROW(TestedDecoder::readDU(Extractor, DC_Root, AC_Root),
                    InvalidJPEGDataException);
}

} // namespace

BOOST_AUTO_TEST_CASE(readDU_Y1) {
  const std::array<uint8_t, 5> Source = {0b10101110, 0b11100111, 0b01100001,
                                         0b11110010, 0b0};
  const auto Expected = Y1_Table();
  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Y2) {
  const std::array<uint8_t, 3> Source = {0b00110111, 0b10101010, 0b01000000};
  const auto Expected = Y2_Table();
  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Y3) {
  const std::array<uint8_t, 4> Source = {0b01010000, 0b10101011, 0b10100000,
                                         0b10000000};
  const auto Expected = Y3_Table();
  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Y4) {
  const std::array<uint8_t, 4> Source = {0b00111100, 0b10000010, 0b11001000,
                                         0b01000000};
  const auto Expected = Y4_Table();

  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Cb) {
  const std::array<uint8_t, 2> Source = {0b10001011, 0b00000000};
  const auto Expected = Cb_Table();
  testReadDU(DC_Tree_1(), AC_Tree_1(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Cr) {
  const std::array<uint8_t, 3> Source = {0b00111011, 0b10010111, 0b111};
  const auto Expected = Cr_Table();
  testReadDU(DC_Tree_1(), AC_Tree_1(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_ThrowException) {
  // clang-format off
  auto DC_Root =
    HuffmanTree::Builder{}
      .left( 0x10 )
      .end()
    .done();
  // clang-format on

  // clang-format off
  auto AC_Root =
    HuffmanTree::Builder{}
      .left( 0x88 )
      .right()
        .left( 0xFF )
        .right().left( 0x6F ).end()
      .end()
    .done();
  // clang-format on

  // No space for Null's
  {
    const std::array<uint8_t, 12> Source = {
        0b01111111, 0b11111111, 0b10111111, 0b11101111, 0b11111111, 0b11110111,
        0b11111111, 0b11111011, 0b11111111, 0b11111101, 0b11111111, 0b11111100};

    testReadDUWithException(DC_Root, AC_Root, Source);
  }

  // No space for values
  {
    const std::array<uint8_t, 12> Source = {
        0b01111111, 0b11111111, 0b10111111, 0b11101111, 0b11111111, 0b11110111,
        0b11111111, 0b11111011, 0b11111111, 0b11111110, 0b11111111, 0b11111110};

    testReadDUWithException(DC_Root, AC_Root, Source);
  }
}

BOOST_AUTO_TEST_CASE(readMCU) {
  const std::array<uint8_t, 19> Source = {
      0b10101110, 0b11100111, 0b01100001, 0b11110010, 0b00011011,
      0b11010101, 0b00100010, 0b10000101, 0b01011101, 0b00000100,
      0b00111100, 0b10000010, 0b11001000, 0b01001000, 0b10110001,
      0b11011100, 0b10111111, 0b11111111, 0b11011001};
  const auto Buffer = charVectorForBuffer(Source);
  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
  BitExtractor Extractor{InputStream};

  const auto Channels = ChannelsForMCU();

  const std::vector<std::shared_ptr<HuffmanTree::Node>> AC_Tables{AC_Tree_0(),
                                                                  AC_Tree_1()};
  const std::vector<std::shared_ptr<HuffmanTree::Node>> DC_Tables{DC_Tree_0(),
                                                                  DC_Tree_1()};
  const auto DCT = DCT_Table();

  const auto mcu =
      TestedDecoder::readMCU(Extractor, DCT, Channels, AC_Tables, DC_Tables);

  const auto Expected_Cs1_1 = Cs1_1_Table();
  const auto Expected_Cs1_2 = Cs1_2_Table();
  const auto Expected_Cs1_3 = Cs1_3_Table();
  const auto Expected_Cs1_4 = Cs1_4_Table();

  BOOST_REQUIRE_EQUAL(mcu.Cs1.size(), 4);
  BOOST_CHECK_EQUAL(Expected_Cs1_1, mcu.Cs1.at(0));
  BOOST_CHECK_EQUAL(Expected_Cs1_2, mcu.Cs1.at(1));
  BOOST_CHECK_EQUAL(Expected_Cs1_3, mcu.Cs1.at(2));
  BOOST_CHECK_EQUAL(Expected_Cs1_4, mcu.Cs1.at(3));

  const auto Expected_Cb = Cb_Table();
  BOOST_REQUIRE_EQUAL(mcu.Cs2.size(), 1);
  BOOST_CHECK_EQUAL(Expected_Cb, mcu.Cs2.at(0));

  const auto Expected_Cr = Cr_Table();
  BOOST_REQUIRE_EQUAL(mcu.Cs3.size(), 1);
  BOOST_CHECK_EQUAL(Expected_Cr, mcu.Cs3.at(0));
}

BOOST_AUTO_TEST_CASE(readMCU_ThrowException) {
  const auto checkThrow = [](const auto &DCT, const auto &Channels) {
    const std::vector<std::shared_ptr<HuffmanTree::Node>> AC_Tables{
        AC_Tree_0(), AC_Tree_1()};
    const std::vector<std::shared_ptr<HuffmanTree::Node>> DC_Tables{
        DC_Tree_0(), DC_Tree_1()};

    const std::array<uint8_t, 19> Source = {
        0b10101110, 0b11100111, 0b01100001, 0b11110010, 0b00011011,
        0b11010101, 0b00100010, 0b10000101, 0b01011101, 0b00000100,
        0b00111100, 0b10000010, 0b11001000, 0b01001000, 0b10110001,
        0b11011100, 0b10111111, 0b11111111, 0b11011001};
    const auto Buffer = charVectorForBuffer(Source);
    ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
    ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
    BitExtractor Extractor{InputStream};
    BOOST_CHECK_THROW(
        TestedDecoder::readMCU(Extractor, DCT, Channels, AC_Tables, DC_Tables),
        InvalidJPEGDataException);
  };

  auto DCT = DCT_Table();

  DCT.Components.push_back({4, 1, 1, 1});

  auto Channels = ChannelsForMCU();

  Channels[2].Id = 5;
  checkThrow(DCT, Channels);

  Channels[2].Id = 4;
  checkThrow(DCT, Channels);
}

BOOST_AUTO_TEST_CASE(quantMCU) {
  TestedDecoder::MinimumCodedUnit MCU;

  MCU.Cs1.push_back(Cs1_1_Table());
  MCU.Cs1.push_back(Cs1_2_Table());
  MCU.Cs1.push_back(Cs1_3_Table());
  MCU.Cs1.push_back(Cs1_4_Table());

  MCU.Cs2.push_back(Cb_Table());
  MCU.Cs3.push_back(Cr_Table());

  const auto Components = ComponentsForDCT();

  const auto DQT1 = DefineQuantizationTable1();
  const auto DQT2 = DefineQuantizationTable2();

  const auto QuantedMCU =
      TestedDecoder::quantMCU(std::move(MCU), Components, {DQT1, DQT2});

  const auto Expected_Cs1_1 = Quanted_Cs1_1_Table();
  const auto Expected_Cs1_2 = Quanted_Cs1_2_Table();
  const auto Expected_Cs1_3 = Quanted_Cs1_3_Table();
  const auto Expected_Cs1_4 = Quanted_Cs1_4_Table();

  BOOST_REQUIRE_EQUAL(QuantedMCU.Cs1.size(), 4);
  BOOST_CHECK_EQUAL(Expected_Cs1_1, QuantedMCU.Cs1.at(0));
  BOOST_CHECK_EQUAL(Expected_Cs1_2, QuantedMCU.Cs1.at(1));
  BOOST_CHECK_EQUAL(Expected_Cs1_3, QuantedMCU.Cs1.at(2));
  BOOST_CHECK_EQUAL(Expected_Cs1_4, QuantedMCU.Cs1.at(3));

  const auto Expected_Cb = Quanted_Cb_Table();
  BOOST_REQUIRE_EQUAL(QuantedMCU.Cs2.size(), 1);
  BOOST_CHECK_EQUAL(Expected_Cb, QuantedMCU.Cs2.at(0));

  const auto Expected_Cr = Quanted_Cr_Table();
  BOOST_REQUIRE_EQUAL(QuantedMCU.Cs3.size(), 1);
  BOOST_CHECK_EQUAL(Expected_Cr, QuantedMCU.Cs3.at(0));
}

BOOST_AUTO_TEST_CASE(reverseDQT_Y1) {
  const auto Src = Quanted_Cs1_1_Table();
  const auto Expected = Reversed_Y1();
  const auto Reversed = TestedDecoder::reverseDQT(Src);
  BOOST_REQUIRE_EQUAL(Expected, Reversed);
}

BOOST_AUTO_TEST_CASE(reverseDQT_Y2) {
  const auto Src = Quanted_Cs1_2_Table();
  const auto Expected = Reversed_Y2();
  const auto Reversed = TestedDecoder::reverseDQT(Src);
  BOOST_REQUIRE_EQUAL(Expected, Reversed);
}

BOOST_AUTO_TEST_CASE(reverseDQT_Y3) {
  const auto Src = Quanted_Cs1_3_Table();
  const auto Expected = Reversed_Y3();
  const auto Reversed = TestedDecoder::reverseDQT(Src);
  BOOST_REQUIRE_EQUAL(Expected, Reversed);
}

BOOST_AUTO_TEST_CASE(reverseDQT_Y4) {
  const auto Src = Quanted_Cs1_4_Table();
  const auto Expected = Reversed_Y4();
  const auto Reversed = TestedDecoder::reverseDQT(Src);
  BOOST_REQUIRE_EQUAL(Expected, Reversed);
}

BOOST_AUTO_TEST_CASE(reverseDQT_Cb) {
  const auto Src = Quanted_Cb_Table();
  const auto Expected = Reversed_Cb();
  const auto Reversed = TestedDecoder::reverseDQT(Src);
  BOOST_REQUIRE_EQUAL(Expected, Reversed);
}

BOOST_AUTO_TEST_CASE(reverseDQT_Cr) {
  const auto Src = Quanted_Cr_Table();
  const auto Expected = Reversed_Cr();
  const auto Reversed = TestedDecoder::reverseDQT(Src);
  BOOST_REQUIRE_EQUAL(Expected, Reversed);
}

BOOST_AUTO_TEST_CASE(convertYCbCrToRGB) {
  const auto Y1_Raw = Reversed_Y1();
  const auto Y2_Raw = Reversed_Y2();
  const auto Y3_Raw = Reversed_Y3();
  const auto Y4_Raw = Reversed_Y4();

  TestedDecoder::DataUnit Y_Raw(16, 16);

  const auto RowCount = Y_Raw.size1() / 2;
  const auto ColCount = Y_Raw.size2() / 2;

  BOOST_REQUIRE_EQUAL(Y1_Raw.size1(), RowCount);
  BOOST_REQUIRE_EQUAL(Y2_Raw.size1(), RowCount);
  BOOST_REQUIRE_EQUAL(Y3_Raw.size1(), RowCount);
  BOOST_REQUIRE_EQUAL(Y4_Raw.size1(), RowCount);

  BOOST_REQUIRE_EQUAL(Y1_Raw.size2(), ColCount);
  BOOST_REQUIRE_EQUAL(Y2_Raw.size2(), ColCount);
  BOOST_REQUIRE_EQUAL(Y3_Raw.size2(), ColCount);
  BOOST_REQUIRE_EQUAL(Y4_Raw.size2(), ColCount);

  for (std::size_t Row = 0; Row < RowCount; Row++) {
    for (std::size_t Col = 0; Col < ColCount; Col++) {
      Y_Raw(Row, Col) = Y1_Raw(Row, Col);
      Y_Raw(Row, 8 + Col) = Y2_Raw(Row, Col);
      Y_Raw(8 + Row, Col) = Y3_Raw(Row, Col);
      Y_Raw(8 + Row, 8 + Col) = Y4_Raw(Row, Col);
    }
  }

  const auto Y = TestedDecoder::normalizeReversedDQT(std::move(Y_Raw));
  const auto Cb = TestedDecoder::normalizeReversedDQT(Reversed_Cb());
  const auto Cr = TestedDecoder::normalizeReversedDQT(Reversed_Cr());

  const auto [R, G, B] = TestedDecoder::convertYCbCrToRGB(Y, Cb, Cr);

  const auto ExpectedR = RedChannel();
  const auto ExpectedG = GreenChannel();
  const auto ExpectedB = BlueChannel();
  BOOST_REQUIRE_EQUAL(ExpectedR, R);
  BOOST_REQUIRE_EQUAL(ExpectedG, G);
  BOOST_REQUIRE_EQUAL(ExpectedB, B);

  saveRGBToImage(R, G, B, "SOSDecoderTests_convertYCbCrToRGB.tif");
}

BOOST_AUTO_TEST_CASE(Invoke) {
  const std::array<uint8_t, 29> Source = {
      0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00,
      0x3f, 0x00, 0xae, 0xe7, 0x61, 0xf2, 0x1b, 0xd5, 0x22, 0x85,
      0x5d, 0x04, 0x3c, 0x82, 0xc8, 0x48, 0xb1, 0xdc, 0xbf};

  DecoderContext Context;
  Context.dct = DCT_Table();
  Context.AC_HuffmanTables = {AC_Tree_0(), AC_Tree_1()};
  Context.DC_HuffmanTables = {DC_Tree_0(), DC_Tree_1()};
  Context.DQT_Vector = {DefineQuantizationTable1(), DefineQuantizationTable2()};

  invokeDecoderWithDataBuffer<SOSDecoder>(Context, Source);
  BOOST_REQUIRE_EQUAL(Context.Image.size(), 1);
  const auto &RGB = Context.Image.at(0);

  const auto ExpectedR = RedChannel();
  const auto ExpectedG = GreenChannel();
  const auto ExpectedB = BlueChannel();
  BOOST_REQUIRE_EQUAL(ExpectedR, RGB.R);
  BOOST_REQUIRE_EQUAL(ExpectedG, RGB.G);
  BOOST_REQUIRE_EQUAL(ExpectedB, RGB.B);

  saveRGBToImage(RGB.R, RGB.G, RGB.B, "SOSDecoderTests_Invoke.tif");
}

BOOST_AUTO_TEST_SUITE_END()
