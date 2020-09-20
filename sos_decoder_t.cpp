#include <boost/test/unit_test.hpp>

#include "sos_decoder.hpp"
#include "testing_utility.hpp"

#include <boost/iostreams/stream.hpp>
#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>

#include <tiffio.h>

//-------------------------------------
BOOST_AUTO_TEST_SUITE(SOSDecoderTests)

namespace ios = boost::iostreams;

class TestedDecoder final : public SOSDecoder {
public:
  using SOSDecoder::Channel;
  using SOSDecoder::MinimumCodedUnit;
  using SOSDecoder::DataUnit;
  using SOSDecoder::locateNodeInHuffmanTree;
  using SOSDecoder::readDU;
  using SOSDecoder::readMCU;
  using SOSDecoder::quantMCU;
  using SOSDecoder::reverseDQT;
  using SOSDecoder::convertYCbCrToRGB;
  using SOSDecoder::normalizeReversedDQT;
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
    boost::numeric::ublas::matrix<int8_t> Table(8, 8);
    Table <<= 2, 0, 3, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

auto Y2_Table() {
    boost::numeric::ublas::matrix<int8_t> Table(8, 8);
    Table <<= -4, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

auto Y3_Table() {
    boost::numeric::ublas::matrix<int8_t> Table(8, 8);
    Table <<= 5, -1, 1, 0, 0, 0, 0, 0, -1, -2, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0,
        0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

auto Y4_Table() {
    boost::numeric::ublas::matrix<int8_t> Table(8, 8);
    Table <<= -4, 2, 2, 1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, -1, -1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

auto Cb_Table() {
    boost::numeric::ublas::matrix<int8_t> Table(8, 8);
    Table <<= -1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

auto Cr_Table() {
    boost::numeric::ublas::matrix<int8_t> Table(8, 8);
    Table <<= 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

auto Cs1_1_Table() {
    TestedDecoder::DataUnit Table(8, 8);
    Table <<= 2, 0, 3, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0,
              0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

auto Cs1_2_Table() {
    TestedDecoder::DataUnit Table(8, 8);
    Table <<= -2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

auto Cs1_3_Table() {
    TestedDecoder::DataUnit Table(8, 8);
    Table <<= 3, -1, 1, 0, 0, 0, 0, 0, -1, -2, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0,
        0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

auto Cs1_4_Table() {
    TestedDecoder::DataUnit Table(8, 8);
    Table <<= -1, 2, 2, 1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, -1, -1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

//    TestedDecoder::DataUnit Table(8, 8);

template<class T = int8_t>
auto Quanted_Cs1_1_Table() {
    boost::numeric::ublas::matrix<T> Table(8,8);
    Table <<=   320, 0, 300, 0, 0, 0, 0, 0, 0, 120, 280, 0, 0, 0, 0, 0, 0, -130,
                        -160, 0, 0, 0, 0, 0, 140, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

template<class T = int8_t>
auto Quanted_Cs1_2_Table() {
//    TestedDecoder::DataUnit Table(8, 8);
    boost::numeric::ublas::matrix<T> Table(8,8);
    Table <<= -320, 110, 100, 160, 0, 0, 0, 0, 0, 0, 140, 0, 0, 0, 0, 0, 0, -130,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

template<class T = int8_t>
auto Quanted_Cs1_3_Table() {
//    TestedDecoder::DataUnit Table(8, 8);
    boost::numeric::ublas::matrix<T> Table(8,8);
    Table <<=   480, -110, 100, 0, 0, 0, 0, 0, -120, -240, -140, 0, 0, 0, 0, 0, 0,
                        -130, 0, 0, 0, 0, 0, 0, -140, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

template<class T = int8_t>
auto Quanted_Cs1_4_Table() {
//    TestedDecoder::DataUnit Table(8, 8);
    boost::numeric::ublas::matrix<T> Table(8,8);
    Table <<=  -160, 220, 200, 160, 0, 0, 0, 0, -120, 0, -140, 0, 0, 0, 0, 0, -140,
                        -130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

template<class T = int8_t>
auto Quanted_Cb_Table() {
//    boost::numeric::ublas::matrix<T> Table(8, 8);
    boost::numeric::ublas::matrix<T> Table(8,8);
    Table <<= -170, 0, 0, 0, 0, 0, 0, 0, 180, 210, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

template<class T = int8_t>
auto Quanted_Cr_Table() {
//    TestedDecoder::DataUnit Table(8, 8);
    boost::numeric::ublas::matrix<T> Table(8,8);
    Table <<= 0, 0, 0, 0, 0, 0, 0, 0, 180, -210, 0, 0, 0, 0, 0, 0, 240, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    return Table;
}

auto Reversed_Y1() {
    boost::numeric::ublas::matrix<int16_t> Table(8, 8);
    Table <<= 138, 92, 27, -17, -17, 28, 93, 139, 136, 82, 5, -51, -55, -8, 61,
        111, 143, 80, -9, -77, -89, -41, 32, 86, 157, 95, 6, -62, -76, -33, 36,
        86, 147, 103, 37, -12, -21, 11, 62, 100, 87, 72, 50, 36, 37, 55, 79, 95,
        -10, 5, 31, 56, 71, 73, 68, 62, -87, -50, 6, 56, 79, 72, 48, 29;
    return Table;
}

auto Reversed_Cb() {
    boost::numeric::ublas::matrix<int16_t> Table(8, 8);
    Table <<= 60, 52, 38, 20, 0, -18, -32, -40, 48, 41, 29, 13, -3, -19, -31, -37,
              25, 20, 12, 2, -9, -19, -27, -32, -4, -6, -9, -13, -17, -20, -23, -25,
              -37, -35, -33, -29, -25, -21, -18, -17, -67, -63, -55, -44, -33, -22, -14,
              -10, -90, -84, -71, -56, -39, -23, -11, -4, -102, -95, -81, -62, -42, -23,
              -9, -1;
    return Table;
}

auto Reversed_Cr() {
    boost::numeric::ublas::matrix<int16_t> Table(8, 8);
    Table <<= 19, 27, 41, 60, 80, 99, 113, 120, 0, 6, 18, 34, 51, 66, 78, 85, -27,
        -22, -14, -4, 7, 17, 25, 30, -43, -41, -38, -34, -30, -27, -24, -22, -35,
        -36, -39, -43, -47, -51, -53, -55, -5, -9, -17, -28, -39, -50, -58, -62,
        32, 26, 14, -1, -18, -34, -46, -53, 58, 50, 36, 18, -2, -20, -34, -42;
    return Table;
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

BOOST_AUTO_TEST_CASE(locateNodeInHuffmanTree_exception) {
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
                boost::numeric::ublas::matrix<int8_t> const &Expected) {
  const auto Buffer = charVectorForBuffer(Source);
  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
  BitExtractor Extractor{InputStream};
  const auto DU = TestedDecoder::readDU(Extractor, DC_Root, AC_Root);
  printMatrix(DU);
  BOOST_REQUIRE_EQUAL(DU, Expected);
}

template <class Container>
void testReadDUWithException(
        std::shared_ptr<HuffmanTree::Node> const &DC_Root,
        std::shared_ptr<HuffmanTree::Node> const &AC_Root,
        Container const &Source) {
    const auto Buffer = charVectorForBuffer(Source);
    ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
    ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
    BitExtractor Extractor{InputStream};
    BOOST_CHECK_THROW(TestedDecoder::readDU(Extractor, DC_Root, AC_Root), std::exception);
}

BOOST_AUTO_TEST_CASE(readDU_Y1) {
  std::array<uint8_t, 5> const Source{0b10101110, 0b11100111, 0b01100001,
                                      0b11110010, 0b0};
  const auto Expected = Y1_Table();
  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Y2) {
  std::array<uint8_t, 3> const Source{0b00110111, 0b10101010, 0b01000000};
  const auto Expected = Y2_Table();
  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Y3) {
  std::array<uint8_t, 4> const Source{0b01010000, 0b10101011, 0b10100000,
                                      0b10000000};
    const auto Expected = Y3_Table();
  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Y4) {
  std::array<uint8_t, 4> const Source{0b00111100, 0b10000010, 0b11001000,
                                      0b01000000};
const auto Expected = Y4_Table();

  testReadDU(DC_Tree_0(), AC_Tree_0(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Cb) {
  std::array<uint8_t, 2> const Source{0b10001011, 0b00000000};
const auto Expected = Cb_Table();
  testReadDU(DC_Tree_1(), AC_Tree_1(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_Cr) {
  std::array<uint8_t, 3> const Source{0b00111011, 0b10010111, 0b111};
const auto Expected = Cr_Table();
  testReadDU(DC_Tree_1(), AC_Tree_1(), Source, Expected);
}

BOOST_AUTO_TEST_CASE(readDU_ThrowException) {
  auto DC_Root =
  // clang-format off
    HuffmanTree::Node::Builder{}
        .left( 0x10 )
        .end()
    .done();
  // clang-format on

  auto AC_Root =
  // clang-format off
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
      // clang-format off
      std::array<uint8_t, 12> const Source{
          0b01111111,
          0b11111111,
          0b10111111,
          0b11101111,
          0b11111111,
          0b11110111,
          0b11111111,
          0b11111011,
          0b11111111,
          0b11111101,
          0b11111111,
          0b11111100
      };
      // clang-format on

      testReadDUWithException(DC_Root, AC_Root, Source);
  }

  // No space for values
  {
    std::array<uint8_t, 12> const Source{
          0b01111111,
          0b11111111,
          0b10111111,
          0b11101111,
          0b11111111,
          0b11110111,
          0b11111111,
          0b11111011,
          0b11111111,
          0b11111110,
          0b11111111,
          0b11111110
      };

    testReadDUWithException(DC_Root, AC_Root, Source);
  }
}

BOOST_AUTO_TEST_CASE(readMCU) {
    std::array<uint8_t, 19> const Source {
        0b10101110, 0b11100111, 0b01100001, 0b11110010,
        0b00011011, 0b11010101, 0b00100010, 0b10000101,
        0b01011101, 0b00000100, 0b00111100, 0b10000010,
        0b11001000, 0b01001000, 0b10110001, 0b11011100,
        0b10111111, 0b11111111, 0b11011001
    };
    const auto Buffer = charVectorForBuffer(Source);
    ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
    ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
    BitExtractor Extractor{InputStream};

    DCTTable DCT;

    DCT.Precision = 8;
    DCT.Width = 16;
    DCT.Height = 16;

    DCT.Components.resize(3);

    DCT.Components[0].Id = 1;
    DCT.Components[0].H = 2;
    DCT.Components[0].V = 2;
    DCT.Components[0].DQT_Id = 0;

    DCT.Components[1].Id = 2;
    DCT.Components[1].H = 1;
    DCT.Components[1].V = 1;
    DCT.Components[1].DQT_Id = 1;

    DCT.Components[2].Id = 3;
    DCT.Components[2].H = 1;
    DCT.Components[2].V = 1;
    DCT.Components[2].DQT_Id = 1;

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

    std::vector<std::shared_ptr<HuffmanTree::Node>> AC_Tables{
        AC_Tree_0(), AC_Tree_1()
    };
    std::vector<std::shared_ptr<HuffmanTree::Node>> DC_Tables{
        DC_Tree_0(), DC_Tree_1()
    };

    const auto mcu = TestedDecoder::readMCU(Extractor, DCT, Channels,
                                         AC_Tables, DC_Tables);

    const auto Expected_Cs1_1 = Cs1_1_Table();
    const auto Expected_Cs1_2 = Cs1_2_Table();
    const auto Expected_Cs1_3 = Cs1_3_Table();
    const auto Expected_Cs1_4 = Cs1_4_Table();

    BOOST_REQUIRE_EQUAL(mcu.Cs1.size(), 4);
    BOOST_REQUIRE_EQUAL(Expected_Cs1_1, mcu.Cs1.at(0));
    BOOST_REQUIRE_EQUAL(Expected_Cs1_2, mcu.Cs1.at(1));
    BOOST_REQUIRE_EQUAL(Expected_Cs1_3, mcu.Cs1.at(2));
    BOOST_REQUIRE_EQUAL(Expected_Cs1_4, mcu.Cs1.at(3));

    const auto Expected_Cb = Cb_Table();
    BOOST_REQUIRE_EQUAL(mcu.Cs2.size(), 1);
    BOOST_REQUIRE_EQUAL(Expected_Cb, mcu.Cs2.at(0));

    const auto Expected_Cr = Cr_Table();
    BOOST_REQUIRE_EQUAL(mcu.Cs3.size(), 1);
    BOOST_REQUIRE_EQUAL(Expected_Cr, mcu.Cs3.at(0));
}

BOOST_AUTO_TEST_CASE(readMCU_ThrowException) {
    const auto checkThrow = [](const auto& DCT, const auto& Channels){
        std::vector<std::shared_ptr<HuffmanTree::Node>> const AC_Tables{
            AC_Tree_0(), AC_Tree_1()
        };
        std::vector<std::shared_ptr<HuffmanTree::Node>> const DC_Tables{
            DC_Tree_0(), DC_Tree_1()
        };

        std::array<uint8_t, 19> const Source {
            0b10101110, 0b11100111, 0b01100001, 0b11110010,
            0b00011011, 0b11010101, 0b00100010, 0b10000101,
            0b01011101, 0b00000100, 0b00111100, 0b10000010,
            0b11001000, 0b01001000, 0b10110001, 0b11011100,
            0b10111111, 0b11111111, 0b11011001
        };
        const auto Buffer = charVectorForBuffer(Source);
        ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
        ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
        BitExtractor Extractor{InputStream};
        BOOST_CHECK_THROW(TestedDecoder::readMCU(Extractor, DCT, Channels, AC_Tables, DC_Tables), std::exception);
    };

    DCTTable DCT;

    DCT.Precision = 8;
    DCT.Width = 16;
    DCT.Height = 16;

    DCT.Components.resize(4);

    DCT.Components[0].Id = 1;
    DCT.Components[0].H = 2;
    DCT.Components[0].V = 2;
    DCT.Components[0].DQT_Id = 0;

    DCT.Components[1].Id = 2;
    DCT.Components[1].H = 1;
    DCT.Components[1].V = 1;
    DCT.Components[1].DQT_Id = 1;

    DCT.Components[2].Id = 3;
    DCT.Components[2].H = 1;
    DCT.Components[2].V = 1;
    DCT.Components[2].DQT_Id = 1;

    DCT.Components[3].Id = 4;
    DCT.Components[3].H = 1;
    DCT.Components[3].V = 1;
    DCT.Components[3].DQT_Id = 1;

    std::vector<TestedDecoder::Channel> Channels;
    Channels.resize(3);

    Channels[0].Id = 1;
    Channels[0].AC_Id = 0;
    Channels[0].DC_Id = 0;

    Channels[1].Id = 2;
    Channels[1].AC_Id = 1;
    Channels[1].DC_Id = 1;

    Channels[2].Id = 5;
    Channels[2].AC_Id = 1;
    Channels[2].DC_Id = 1;

    checkThrow(DCT, Channels);

    Channels[2].Id = 4;

    checkThrow(DCT, Channels);
}

BOOST_AUTO_TEST_CASE(quantMCU) {
  TestedDecoder::MinimumCodedUnit mcu;

  mcu.Cs1.push_back(Cs1_1_Table());
  mcu.Cs1.push_back(Cs1_2_Table());
  mcu.Cs1.push_back(Cs1_3_Table());
  mcu.Cs1.push_back(Cs1_4_Table());

  mcu.Cs2.push_back(Cb_Table());
  mcu.Cs3.push_back(Cr_Table());

  std::vector<DCTComponent> components;

  components.resize(3);

  components[0].Id = 1;
  components[0].H = 2;
  components[0].V = 2;
  components[0].DQT_Id = 0;

  components[1].Id = 2;
  components[1].H = 1;
  components[1].V = 1;
  components[1].DQT_Id = 1;

  components[2].Id = 3;
  components[2].H = 1;
  components[2].V = 1;
  components[2].DQT_Id = 1;

  boost::numeric::ublas::matrix<uint16_t> DQT1(8, 8);
  DQT1 <<= 0xA0, 0x6E, 0x64, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF, 0x78, 0x78, 0x8C,
      0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0x8C, 0x82, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF,
      0xFF, 0x8C, 0xAA, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB4, 0xDC, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF;

  boost::numeric::ublas::matrix<uint16_t> DQT2(8, 8);
  DQT2 <<= 0xaa, 0xb4, 0xf0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xb4, 0xd2, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xf0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF;

  mcu = TestedDecoder::quantMCU( std::move( mcu ), components, {DQT1, DQT2});

  const auto Expected_Cs1_1 = Quanted_Cs1_1_Table();
  const auto Expected_Cs1_2 = Quanted_Cs1_2_Table();
  const auto Expected_Cs1_3 = Quanted_Cs1_3_Table();
  const auto Expected_Cs1_4 = Quanted_Cs1_4_Table();

  BOOST_REQUIRE_EQUAL(mcu.Cs1.size(), 4);
  BOOST_REQUIRE_EQUAL(Expected_Cs1_1, mcu.Cs1.at(0));
  BOOST_REQUIRE_EQUAL(Expected_Cs1_2, mcu.Cs1.at(1));
  BOOST_REQUIRE_EQUAL(Expected_Cs1_3, mcu.Cs1.at(2));
  BOOST_REQUIRE_EQUAL(Expected_Cs1_4, mcu.Cs1.at(3));

  const auto Expected_Cb = Quanted_Cb_Table();
  BOOST_REQUIRE_EQUAL(mcu.Cs2.size(), 1);
  BOOST_REQUIRE_EQUAL(Expected_Cb, mcu.Cs2.at(0));

  const auto Expected_Cr = Quanted_Cr_Table();
  BOOST_REQUIRE_EQUAL(mcu.Cs3.size(), 1);
  BOOST_REQUIRE_EQUAL(Expected_Cr, mcu.Cs3.at(0));
}

BOOST_AUTO_TEST_CASE(reverseDQT_Y1) {
  const auto Src = Quanted_Cs1_1_Table<int16_t>();
  const auto Expected = Reversed_Y1();
  const auto Reversed = TestedDecoder::reverseDQT(Src);
  BOOST_REQUIRE_EQUAL(Expected, Reversed);
}

BOOST_AUTO_TEST_CASE(reverseDQT_XXX) {
    const auto q1 = Quanted_Cs1_2_Table();
    const auto q2 = Quanted_Cs1_3_Table();
    const auto q3 = Quanted_Cs1_4_Table();
    const auto r1 = TestedDecoder::reverseDQT(q1);
    const auto r2 = TestedDecoder::reverseDQT(q2);
    const auto r3 = TestedDecoder::reverseDQT(q3);
    printMatrix(r1);
    printMatrix(r2);
    printMatrix(r3);
}

BOOST_AUTO_TEST_CASE(reverseDQT_Y2) {
  const auto Src = Quanted_Cs1_1_Table<int16_t>();
  const auto Expected = Reversed_Y1();
  const auto Reversed = TestedDecoder::reverseDQT(Src);
  BOOST_REQUIRE_EQUAL(Expected, Reversed);
}

BOOST_AUTO_TEST_CASE(reverseDQT_cb) {
  const auto Src = Quanted_Cb_Table<int16_t>();
  const auto Expected = Reversed_Cb();
  const auto Reversed = TestedDecoder::reverseDQT(Src);
  BOOST_REQUIRE_EQUAL(Expected, Reversed);
}

BOOST_AUTO_TEST_CASE(reverseDQT_cr) {
  const auto Src = Quanted_Cr_Table<int16_t>();

  boost::numeric::ublas::matrix<int16_t> Expected(8, 8);
  Expected <<=  19,  27,  41,  60,  80,  99, 113, 120,
          0,   6,  18,  34,  51,  66,  78,  85,
        -27, -22, -14,  -4,   7,  17,  25,  30,
        -43, -41, -38, -34, -30, -27, -24, -22,
        -35, -36, -39, -43, -47, -51, -53, -55,
         -5,  -9, -17, -28, -39, -50, -58, -62,
         32,  26,  14,  -1, -18, -34, -46, -53,
         58,  50,  36,  18, -2, -20, -34, -42;

  const auto Reversed = TestedDecoder::reverseDQT(Src);
  BOOST_REQUIRE_EQUAL(Expected, Reversed);
}

BOOST_AUTO_TEST_CASE(Invoke) {
    std::array<uint8_t, 29> const Source{
        0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11,
        0x03, 0x11, 0x00, 0x3f, 0x00, 0xae, 0xe7,
        0x61, 0xf2, 0x1b, 0xd5, 0x22, 0x85, 0x5d,
        0x04, 0x3c, 0x82, 0xc8, 0x48, 0xb1, 0xdc,
        0xbf
    };

    const auto width = 16;
    const auto height = 16;

    Context Ctx;

    Ctx.dct.Precision = 8;
    Ctx.dct.Width = width;
    Ctx.dct.Height = height;

    Ctx.dct.Components.resize(3);

    Ctx.dct.Components[0].Id = 1;
    Ctx.dct.Components[0].H = 2;
    Ctx.dct.Components[0].V = 2;
    Ctx.dct.Components[0].DQT_Id = 0;

    Ctx.dct.Components[1].Id = 2;
    Ctx.dct.Components[1].H = 1;
    Ctx.dct.Components[1].V = 1;
    Ctx.dct.Components[1].DQT_Id = 1;

    Ctx.dct.Components[2].Id = 3;
    Ctx.dct.Components[2].H = 1;
    Ctx.dct.Components[2].V = 1;
    Ctx.dct.Components[2].DQT_Id = 1;

    Ctx.AC_HuffmanTables = { AC_Tree_0(), AC_Tree_1() };
    Ctx.DC_HuffmanTables = { DC_Tree_0(), DC_Tree_1() };

    boost::numeric::ublas::matrix<uint16_t> DQT1(8, 8);
    DQT1 <<=    0xA0, 0x6E, 0x64, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF,
                0x78, 0x78, 0x8C, 0xBE, 0xFF, 0xFF, 0xFF, 0xFF,
                0x8C, 0x82, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF,
                0x8C, 0xAA, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xB4, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF;
    boost::numeric::ublas::matrix<uint16_t> DQT2(8, 8);
    DQT2 <<=    0xAA, 0xB4, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xB4, 0xD2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF;

    Ctx.DQT_Vector = { DQT1, DQT2 };

    std::cout << "????" << std::endl;

    invokeDecoderWithDataBuffer<SOSDecoder>(Ctx, Source);

    std::cout << "!!!!" << std::endl;

    auto createRGB = [](int r, int g, int b) -> unsigned long {
        return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
    };

    //---------------writing part------------------

    uint32* buffer = (uint32 *)malloc(width*height * sizeof(uint32));

    TIFF *image = TIFFOpen("input.tif", "w");
    TIFFSetField(image, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(image, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(image, TIFFTAG_BITSPERSAMPLE, 32);
    TIFFSetField(image, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(image, TIFFTAG_ROWSPERSTRIP, 1);
    TIFFSetField(image, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(image, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(image, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(image, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
    TIFFSetField(image, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

    auto scan_line = (uint32 *)malloc(width*(sizeof(uint32)));

    int cx = 0;

    std::cout << "size" << Ctx.Image.size() << "height" << height;
    auto rgb = Ctx.Image.at( cx );

    for (int i = 0; i < height; i++) {
        for ( auto j = 0; j < width; ++j ) {
            scan_line[j] = createRGB(rgb.R(i,j), rgb.G(i,j), rgb.B(i,j));
        }
        TIFFWriteScanline(image, scan_line, i, 0);
    }

    TIFFClose(image);
    free(buffer);
    free(scan_line);
}

#if 0

BOOST_AUTO_TEST_CASE(convertYCbCrToRGB_LOLOL) {
    boost::numeric::ublas::matrix<int16_t> MatR(8, 8);
    MatR <<=      255, 249, 195, 149, 169, 215, 255, 255,
                  255, 238, 172, 116, 131, 179, 255, 255,
                  255, 209, 127,  58,  64, 112, 209, 255,
                  255, 224, 143,  73,  76, 120, 212, 255,
                  217, 193, 134,  84,  86, 118, 185, 223,
                  177, 162, 147, 132, 145, 162, 201, 218,
                   57,  74, 101, 125, 144, 146, 147, 142,
                    0, 18,  76,  125, 153, 146, 128, 108;

    std::cout <<  "C++" << std::endl;

    for (  auto it = MatR.begin2(); it != MatR.end2(); it++ ) {
        std::cout << *it << ' ';
    }

    std::cout <<  "C++" << std::endl;

//    for (  auto it = MatR.begin1(); it != MatR.end1(); it++ ) {
//        std::cout << *it << std::endl;
//        it.begin2();
//    }
}

#endif

#if 0

BOOST_AUTO_TEST_CASE(convertYCbCrToRGB) {
  const auto Y = TestedDecoder::normalizeReversedDQT( Reversed_Y1() );
  boost::numeric::ublas::matrix<int16_t> Cb_Raw(8, 8);
  Cb_Raw <<=  60, 60, 52, 52, 38, 38,  20,   20,
              60, 60, 52, 52, 38, 38,  20,   20,
              48, 48, 41, 41, 29, 29,  13,   13,
              48, 48, 41, 41, 29, 29,  13,   13,
              25, 25, 20, 20, 12, 12,   2,    2,
              25, 25, 20, 20, 12, 12,   2,    2,
              -4, -4, -6, -6, -9, -9, -13,  -13,
              -4, -4, -6, -6, -9, -9, -13,  -13;
  boost::numeric::ublas::matrix<int16_t> Cr_Raw(8, 8);
    Cr_Raw <<= 19, 19,   27,  27,  41, 41,  60, 60,
            19, 19,   27,  27,  41, 41,  60, 60,
            0,  0,     6,   6,  18, 18,  34, 34,
            0,  0,     6,   6,  18, 18,  34, 34,
            -22,-22, -14, -14,  -4, -4,   7,  7,
            -22,-22, -14, -14,  -4, -4,   7,  7,
            -41,-41, -38, -38, -34,-34, -30,-30,
            -41,-41, -38, -38, -34,-34, -30,-30;

  const auto Cb = TestedDecoder::normalizeReversedDQT( std::move( Cb_Raw ) );
  const auto Cr = TestedDecoder::normalizeReversedDQT( std::move( Cr_Raw ) );
//  const auto Cb = TestedDecoder::normalizeReversedDQT( Reversed_Cb() );
//  const auto Cr = TestedDecoder::normalizeReversedDQT( Reversed_Cr() );

  const auto res = TestedDecoder::convertYCbCrToRGB(Y, Cb, Cr);

  // по хорошему такие данные, но у нас что о не то с точностью =[
//    boost::numeric::ublas::matrix<int16_t> ExpectedR(8, 8);
//    ExpectedR <<= 255, 249, 195, 149, 169, 215, 255, 255,
//                  255, 238, 172, 116, 131, 179, 255, 255,
//                  255, 209, 127,  58,  64, 112, 209, 255,
//                  255, 224, 143,  73,  76, 120, 212, 255,
//                  217, 193, 134,  84,  86, 118, 185, 223,
//                  177, 162, 147, 132, 145, 162, 201, 218,
//                   57,  74, 101, 125, 144, 146, 147, 142,
//                    0, 18,  76,  125, 153, 146, 128, 108;

//    boost::numeric::ublas::matrix<int16_t> ExpectedG(8, 8);
//    ExpectedG <<= 220, 186, 118,  72,  67, 113, 172, 205,
//                  220, 175,  95,  39,  29,  77, 139, 190,
//                  238, 192, 100,  31,  16,  64, 132, 185,
//                  238, 207, 116,  46,  28,  72, 135, 186,
//                  255, 242, 175, 125, 113, 145, 193, 231,
//                  226, 211, 188, 173, 172, 189, 209, 226,
//                  149, 166, 192, 216, 230, 232, 225, 220,
//                   73, 110, 167, 216, 239, 232, 206, 186;

//    boost::numeric::ublas::matrix<int16_t> ExpectedB(8, 8);
//    ExpectedB <<= 255, 255, 250, 204, 179, 225, 255, 255,
//                  255, 255, 227, 171, 141, 189, 224, 255,
//                  255, 255, 193, 124,  90, 138, 186, 239,
//                  255, 255, 209, 139, 102, 146, 189, 240,
//                  255, 255, 203, 153, 130, 162, 195, 233,
//                  255, 244, 216, 201, 189, 206, 211, 228,
//                  108, 125, 148, 172, 183, 185, 173, 168,
//                  32,  69, 123, 172, 192, 185, 154, 134;

  boost::numeric::ublas::matrix<int16_t> ExpectedR(8, 8);
  ExpectedR <<=     0xFF,      0xF7,      0xC1,      0x95,      0xA8,      0xD5,      0xFF,      0xFF,
                    0xFF      ,0xED      ,0xAB      ,0x73      ,0x82      ,0xB1      ,0xFF      ,0xFF,
                    0xFF      ,0xD0      ,0x7F      ,0x3B      ,0x40      ,0x70      ,0xD0      ,0xFF,
                    0xFF      ,0xDF      ,0x8E      ,0x4A      ,0x4D      ,0x78      ,0xD4      ,0xFF,
                    0xD9      ,0xC1      ,0x86      ,0x55      ,0x57      ,0x77      ,0xB8      ,0xDE,
                    0xB1      ,0xA2      ,0x93      ,0x85      ,0x91      ,0xA3      ,0xC9      ,0xD9,
                    0x3A      ,0x49      ,0x66      ,0x7F      ,0x92      ,0x94      ,0x94      ,0x8E,
                    0x0       ,0x12      ,0x4D      ,0x7F      ,0x9A      ,0x93      ,0x80      ,0x6D;

  boost::numeric::ublas::matrix<int16_t> ExpectedG(8, 8);
  ExpectedG <<=     0xDD      , 0xBA      , 0x76      ,0x4A      ,0x45      ,0x72      ,0xAB      ,0xCD,
                    0xDD      , 0xB0      , 0x60      ,0x28      ,0x1F      ,0x4E      ,0x8B      ,0xBD,
                    0xEE      , 0xBF      , 0x65      ,0x21      ,0x10      ,0x40      ,0x83      ,0xB9,
                    0xEE      , 0xCE      , 0x74      ,0x30      ,0x1D      ,0x48      ,0x87      ,0xB9,
                    0xFF      , 0xF2      , 0xAE      ,0x7D      ,0x71      ,0x91      ,0xC0      ,0xE6,
                    0xE2      , 0xD3      , 0xBB      ,0xAD      ,0xAB      ,0xBD      ,0xD1      ,0xE1,
                    0x96      , 0xA5      , 0xBE      ,0xD7      ,0xE5      ,0xE7      ,0xE1      ,0xDB,
                    0x49      , 0x6E      , 0xA5      ,0xD7      ,0xED      ,0xE6      ,0xCD      ,0xBA;


  boost::numeric::ublas::matrix<int16_t> ExpectedB(8, 8);
  ExpectedB <<= 0xFF,      0xFF,      0xF7,      0xCB,      0xB2,      0xDF,     0xFF,      0xFF,
                0xFF,      0xFF,      0xE1,      0xA9,      0x8C,      0xBB,     0xE0,      0xFF,
                0xFF,      0xFF,      0xC0,      0x7C,      0x5A,      0x8A,     0xB7,      0xED,
                0xFF,      0xFF,      0xCF,      0x8B,      0x67,      0x92,     0xBB,      0xED,
                0xFF,      0xFF,      0xC8,      0x97,      0x80,      0xA0,     0xC2,      0xE8,
                0xFF,      0xF4,      0xD5,      0xC7,      0xBA,      0xCC,     0xD3,      0xE3,
                0x6F,      0x7E,      0x94,      0xAD,      0xB7,      0xB9,     0xAD,      0xA7,
                0x22,      0x47,      0x7B,      0xAD,      0xBF,      0xB8,     0x99,      0x86;

  printMatrix(std::get<0>(res));
  printMatrix(std::get<1>(res));
  printMatrix(std::get<2>(res));

  BOOST_REQUIRE_EQUAL(ExpectedR, std::get<0>(res));
  BOOST_REQUIRE_EQUAL(ExpectedG, std::get<1>(res));
  BOOST_REQUIRE_EQUAL(ExpectedB, std::get<2>(res));
}

#endif


#if 0

BOOST_AUTO_TEST_CASE(convertYCbCrToRGB) {
//  const auto Y = TestedDecoder::normalizeReversedDQT( Reversed_Y1() );

    boost::numeric::ublas::matrix<int16_t> Y_Raw(16, 16);
Y_Raw <<= 138,  92, 27, -17, -17,  28, 93, 139, /* | */  15, 31, 39,  20, -16, -47, -55, -51,
          136,  82,  5, -51, -55,  -8, 61, 111, /* | */   2, 19, 28,  13, -17, -39, -39, -30,
          143,  80, -9, -77, -89, -41, 32,  86, /* | */ -13,  2, 12,   1, -20, -29, -16,   0,
          157,  95,  6, -62, -76, -33, 36,  86, /* | */ -20, -8, -1, -11, -26, -23,   1,  26,
          147, 103, 37, -12, -21,  11, 62, 100, /* | */ -10, -3, -5, -22, -37, -28,   5,  37,
           87,  72, 50,  36,  37,  55, 79,  95, /* | */  16, 14,  0, -28, -50, -41,  -4,  30,
          -10,   5, 31,  56,  71,  73, 68,  62, /* | */  47, 37, 10, -31, -62, -58, -21,  13,
          -87, -50,  6,  56,  79,  72, 48,  29, /* | */  68, 53, 17, -31, -69, -69, -34,   1,
          /*_______________________________________________________________________________*/
           48,  21, -17,-47, -53, -32, -1,  21, /* | */  35, 39, 32,   6, -20, -26,  -4,  19,
            8, -13, -43,-65, -63, -39, -6,  16, /* | */   6, 15, 16,   0, -21, -23,   0,  23,
          -33, -47, -65,-72, -61, -32,  1,  24, /* | */ -34,-17, -3,  -7, -19, -16,   6,  30,
          -44, -50, -54,-50, -31,  -1, 29,  48, /* | */ -63,-38,-12,  -4,  -9,  -6,  13,  33,
          -25, -24, -20, -9,  10,  34, 57,  71, /* | */ -66,-34,  0,  14,   9,   6,  17,  31,
           -3,   0,   8, 19,  33,  46, 56,  62, /* | */ -40, -6, 32,  45,  34,  19,  17,  24,
            1,   6,  14, 21,  25,  26, 24,  22, /* | */  -2, 32, 70,  79,  58,  30,  16,  14,
           -4,   0,   7, 10,   8,   0, -9, -15, /* | */ 24,  59, 95, 101,  73,  37,  14,   8;

    const auto Y = TestedDecoder::normalizeReversedDQT( std::move( Y_Raw ) );
    const auto Cb = TestedDecoder::normalizeReversedDQT( Reversed_Cb() );
    const auto Cr = TestedDecoder::normalizeReversedDQT( Reversed_Cr() );

    const auto res = TestedDecoder::convertYCbCrToRGB_3(Y, Cb, Cr);
    printMatrix(std::get<0>(res));
    printMatrix(std::get<1>(res));
    printMatrix(std::get<2>(res));
}

#endif

//  // по хорошему такие данные, но у нас что о не то с точностью =[
//    boost::numeric::ublas::matrix<int16_t> ExpectedR(8, 8);
//    ExpectedR <<= 255, 249, 195, 149, 169, 215, 255, 255,
//                  255, 238, 172, 116, 131, 179, 255, 255,
//                  255, 209, 127,  58,  64, 112, 209, 255,
//                  255, 224, 143,  73,  76, 120, 212, 255,
//                  217, 193, 134,  84,  86, 118, 185, 223,
//                  177, 162, 147, 132, 145, 162, 201, 218,
//                   57,  74, 101, 125, 144, 146, 147, 142,
//                    0, 18,  76,  125, 153, 146, 128, 108;

//    boost::numeric::ublas::matrix<int16_t> ExpectedG(8, 8);
//    ExpectedG <<= 220, 186, 118,  72,  67, 113, 172, 205,
//                  220, 175,  95,  39,  29,  77, 139, 190,
//                  238, 192, 100,  31,  16,  64, 132, 185,
//                  238, 207, 116,  46,  28,  72, 135, 186,
//                  255, 242, 175, 125, 113, 145, 193, 231,
//                  226, 211, 188, 173, 172, 189, 209, 226,
//                  149, 166, 192, 216, 230, 232, 225, 220,
//                   73, 110, 167, 216, 239, 232, 206, 186;

//    boost::numeric::ublas::matrix<int16_t> ExpectedB(8, 8);
//    ExpectedB <<= 255, 255, 250, 204, 179, 225, 255, 255,
//                  255, 255, 227, 171, 141, 189, 224, 255,
//                  255, 255, 193, 124,  90, 138, 186, 239,
//                  255, 255, 209, 139, 102, 146, 189, 240,
//                  255, 255, 203, 153, 130, 162, 195, 233,
//                  255, 244, 216, 201, 189, 206, 211, 228,
//                  108, 125, 148, 172, 183, 185, 173, 168,
//                  32,  69, 123, 172, 192, 185, 154, 134;

//  printMatrix(std::get<0>(res));
//  printMatrix(std::get<1>(res));
//  printMatrix(std::get<2>(res));

//  BOOST_REQUIRE_EQUAL(ExpectedR, std::get<0>(res));
//  BOOST_REQUIRE_EQUAL(ExpectedG, std::get<1>(res));
//  BOOST_REQUIRE_EQUAL(ExpectedB, std::get<2>(res));

//  boost::numeric::ublas::matrix<int16_t> ExpectedR(8, 8);
//  ExpectedR <<=     0xFF,      0xF7,      0xC1,      0x95,      0xA8,      0xD5,      0xFF,      0xFF,
//                    0xFF      ,0xED      ,0xAB      ,0x73      ,0x82      ,0xB1      ,0xFF      ,0xFF,
//                    0xFF      ,0xD0      ,0x7F      ,0x3B      ,0x40      ,0x70      ,0xD0      ,0xFF,
//                    0xFF      ,0xDF      ,0x8E      ,0x4A      ,0x4D      ,0x78      ,0xD4      ,0xFF,
//                    0xD9      ,0xC1      ,0x86      ,0x55      ,0x57      ,0x77      ,0xB8      ,0xDE,
//                    0xB1      ,0xA2      ,0x93      ,0x85      ,0x91      ,0xA3      ,0xC9      ,0xD9,
//                    0x3A      ,0x49      ,0x66      ,0x7F      ,0x92      ,0x94      ,0x94      ,0x8E,
//                    0x0       ,0x12      ,0x4D      ,0x7F      ,0x9A      ,0x93      ,0x80      ,0x6D;

//  boost::numeric::ublas::matrix<int16_t> ExpectedG(8, 8);
//  ExpectedG <<=     0xDD      , 0xBA      , 0x76      ,0x4A      ,0x45      ,0x72      ,0xAB      ,0xCD,
//                    0xDD      , 0xB0      , 0x60      ,0x28      ,0x1F      ,0x4E      ,0x8B      ,0xBD,
//                    0xEE      , 0xBF      , 0x65      ,0x21      ,0x10      ,0x40      ,0x83      ,0xB9,
//                    0xEE      , 0xCE      , 0x74      ,0x30      ,0x1D      ,0x48      ,0x87      ,0xB9,
//                    0xFF      , 0xF2      , 0xAE      ,0x7D      ,0x71      ,0x91      ,0xC0      ,0xE6,
//                    0xE2      , 0xD3      , 0xBB      ,0xAD      ,0xAB      ,0xBD      ,0xD1      ,0xE1,
//                    0x96      , 0xA5      , 0xBE      ,0xD7      ,0xE5      ,0xE7      ,0xE1      ,0xDB,
//                    0x49      , 0x6E      , 0xA5      ,0xD7      ,0xED      ,0xE6      ,0xCD      ,0xBA;

//  boost::numeric::ublas::matrix<int16_t> ExpectedB(8, 8);
//  ExpectedB <<= 0xFF,      0xFF,      0xF7,      0xCB,      0xB2,      0xDF,     0xFF,      0xFF,
//                0xFF,      0xFF,      0xE1,      0xA9,      0x8C,      0xBB,     0xE0,      0xFF,
//                0xFF,      0xFF,      0xC0,      0x7C,      0x5A,      0x8A,     0xB7,      0xED,
//                0xFF,      0xFF,      0xCF,      0x8B,      0x67,      0x92,     0xBB,      0xED,
//                0xFF,      0xFF,      0xC8,      0x97,      0x80,      0xA0,     0xC2,      0xE8,
//                0xFF,      0xF4,      0xD5,      0xC7,      0xBA,      0xCC,     0xD3,      0xE3,
//                0x6F,      0x7E,      0x94,      0xAD,      0xB7,      0xB9,     0xAD,      0xA7,
//                0x22,      0x47,      0x7B,      0xAD,      0xBF,      0xB8,     0x99,      0x86;

BOOST_AUTO_TEST_SUITE_END()
