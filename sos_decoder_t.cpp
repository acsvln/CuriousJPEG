#include "sos_decoder.hpp"
#include <boost/test/unit_test.hpp>

#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>

#include "testing_shared_data.hpp"
#include "testing_utility.hpp"

//-------------------------------------
BOOST_AUTO_TEST_SUITE(SOSDecoderTests)

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
  return HuffmanTree::Node::Builder{}
        .left( 0x03 )
        .right().left( 0x02 )
        .end()
    .done();
  // clang-format on
}

auto DC_Tree_1() {
  // clang-format off
  return HuffmanTree::Node::Builder{}
        .left( 0x00 )
        .right().left( 0x01 )
        .end()
    .done();
  // clang-format on
}

auto AC_Tree_0() {
  // clang-format off
  return HuffmanTree::Node::Builder{}
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
  return HuffmanTree::Node::Builder{}
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
    std::shared_ptr<HuffmanTree::Node> const &Root,
    std::array<uint8_t, 1> const &Source,
    std::shared_ptr<HuffmanTree::Node> const &Expected) {
  const auto Buffer = charVectorForBuffer(Source);
  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);

  BitExtractor Extractor{InputStream};
  const auto Located = TestedDecoder::locateNodeInHuffmanTree(Extractor, Root);
  BOOST_REQUIRE_EQUAL(Located, Expected);
}

void testLocateNodeInHuffmanTreeWithException(
    std::shared_ptr<HuffmanTree::Node> const &Root,
    std::array<uint8_t, 1> const &Source) {
  const auto Buffer = charVectorForBuffer(Source);
  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);

  BitExtractor Extractor{InputStream};
  BOOST_CHECK_THROW(TestedDecoder::locateNodeInHuffmanTree(Extractor, Root),
                    std::exception);
}

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

template <class Container>
void testReadDU(std::shared_ptr<HuffmanTree::Node> const &DC_Root,
                std::shared_ptr<HuffmanTree::Node> const &AC_Root,
                Container const &Source,
                TestedDecoder::DataUnit const &Expected) {
  const auto Buffer = charVectorForBuffer(Source);
  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
  BitExtractor Extractor{InputStream};
  const auto DU = TestedDecoder::readDU(Extractor, DC_Root, AC_Root);
  printMatrix(DU);
  BOOST_REQUIRE_EQUAL(DU, Expected);
}

template <class Container>
void testReadDUWithException(std::shared_ptr<HuffmanTree::Node> const &DC_Root,
                             std::shared_ptr<HuffmanTree::Node> const &AC_Root,
                             Container const &Source) {
  const auto Buffer = charVectorForBuffer(Source);
  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
  BitExtractor Extractor{InputStream};
  BOOST_CHECK_THROW(TestedDecoder::readDU(Extractor, DC_Root, AC_Root),
                    std::exception);
}

BOOST_AUTO_TEST_CASE(readDU_Y1) {
  std::array<uint8_t, 5> const Source = {0b10101110, 0b11100111, 0b01100001,
                                         0b11110010, 0b0};
  const auto Expected = Y1_Table();
  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Y2) {
  std::array<uint8_t, 3> const Source = {0b00110111, 0b10101010, 0b01000000};
  const auto Expected = Y2_Table();
  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Y3) {
  std::array<uint8_t, 4> const Source = {0b01010000, 0b10101011, 0b10100000,
                                         0b10000000};
  const auto Expected = Y3_Table();
  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Y4) {
  std::array<uint8_t, 4> const Source = {0b00111100, 0b10000010, 0b11001000,
                                         0b01000000};
  const auto Expected = Y4_Table();

  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Cb) {
  std::array<uint8_t, 2> const Source = {0b10001011, 0b00000000};
  const auto Expected = Cb_Table();
  testReadDU(DC_Tree_1(), AC_Tree_1(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Cr) {
  std::array<uint8_t, 3> const Source = {0b00111011, 0b10010111, 0b111};
  const auto Expected = Cr_Table();
  testReadDU(DC_Tree_1(), AC_Tree_1(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_ThrowException) {
  // clang-format off
  auto DC_Root =
    HuffmanTree::Node::Builder{}
      .left( 0x10 )
      .end()
    .done();
  // clang-format on

  // clang-format off
  auto AC_Root =
    HuffmanTree::Node::Builder{}
      .left( 0x88 )
      .right()
        .left( 0xFF )
        .right().left( 0x6F ).end()
      .end()
    .done();
  // clang-format on

  // No space for Null's
  {
    std::array<uint8_t, 12> const Source = {
        0b01111111, 0b11111111, 0b10111111, 0b11101111, 0b11111111, 0b11110111,
        0b11111111, 0b11111011, 0b11111111, 0b11111101, 0b11111111, 0b11111100};

    testReadDUWithException(DC_Root, AC_Root, Source);
  }

  // No space for values
  {
    std::array<uint8_t, 12> const Source = {
        0b01111111, 0b11111111, 0b10111111, 0b11101111, 0b11111111, 0b11110111,
        0b11111111, 0b11111011, 0b11111111, 0b11111110, 0b11111111, 0b11111110};

    testReadDUWithException(DC_Root, AC_Root, Source);
  }
}

BOOST_AUTO_TEST_CASE(readMCU) {
  std::array<uint8_t, 19> const Source = {
      0b10101110, 0b11100111, 0b01100001, 0b11110010, 0b00011011,
      0b11010101, 0b00100010, 0b10000101, 0b01011101, 0b00000100,
      0b00111100, 0b10000010, 0b11001000, 0b01001000, 0b10110001,
      0b11011100, 0b10111111, 0b11111111, 0b11011001};
  const auto Buffer = charVectorForBuffer(Source);
  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
  BitExtractor Extractor{InputStream};

  const auto Channels = ChannelsForMCU();

  std::vector<std::shared_ptr<HuffmanTree::Node>> const AC_Tables{AC_Tree_0(),
                                                                  AC_Tree_1()};
  std::vector<std::shared_ptr<HuffmanTree::Node>> const DC_Tables{DC_Tree_0(),
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
    std::vector<std::shared_ptr<HuffmanTree::Node>> const AC_Tables{
        AC_Tree_0(), AC_Tree_1()};
    std::vector<std::shared_ptr<HuffmanTree::Node>> const DC_Tables{
        DC_Tree_0(), DC_Tree_1()};

    std::array<uint8_t, 19> const Source = {
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
        std::exception);
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
  saveRGBToImage(R, G, B, "SOSDecoderTests_convertYCbCrToRGB.tif");
}

BOOST_AUTO_TEST_CASE(Invoke) {
  std::array<uint8_t, 29> const Source = {
      0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00,
      0x3f, 0x00, 0xae, 0xe7, 0x61, 0xf2, 0x1b, 0xd5, 0x22, 0x85,
      0x5d, 0x04, 0x3c, 0x82, 0xc8, 0x48, 0xb1, 0xdc, 0xbf};

  Context Ctx;
  Ctx.dct = DCT_Table();
  Ctx.AC_HuffmanTables = {AC_Tree_0(), AC_Tree_1()};
  Ctx.DC_HuffmanTables = {DC_Tree_0(), DC_Tree_1()};
  Ctx.DQT_Vector = {DefineQuantizationTable1(), DefineQuantizationTable2()};

  invokeDecoderWithDataBuffer<SOSDecoder>(Ctx, Source);

  const auto &RGB = Ctx.Image.at(0);
  saveRGBToImage(RGB.R, RGB.G, RGB.B, "SOSDecoderTests_Invoke.tif");
}

BOOST_AUTO_TEST_SUITE_END()
