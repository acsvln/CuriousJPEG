#include <boost/test/unit_test.hpp>

#include "sos_decoder.hpp"
#include "testing_utility.hpp"

#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/iostreams/stream.hpp>

//-------------------------------------
BOOST_AUTO_TEST_SUITE(SOSDecoderTests)

namespace ios = boost::iostreams;

//-------------------------------------
auto DC_Tree_0() -> std::shared_ptr<HuffmanTree::Node> {
  // clang-format off
  return HuffmanTree::Node::Builder{}
        .left( 0x03 )
        .right().left( 0x02 )
        .end()
    .done();
  // clang-format on
}

auto DC_Tree_1() -> std::shared_ptr<HuffmanTree::Node> {
  // clang-format off
  return HuffmanTree::Node::Builder{}
        .left( 0x00 )
        .right().left( 0x01 )
        .end()
    .done();
  // clang-format on
}

auto AC_Tree_0() -> std::shared_ptr<HuffmanTree::Node> {
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

auto AC_Tree_1() -> std::shared_ptr<HuffmanTree::Node> {
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

BOOST_AUTO_TEST_CASE(LocateNodeInTree_DC_0) {
  const std::array<char, 1> data = {(char)0b10000000};

  ios::basic_array_source<char> input_source(data.data(), data.size());
  ios::stream<ios::basic_array_source<char>> input_stream(input_source);

  BitExtractor extractor{input_stream};

  const auto root = DC_Tree_0();

  const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
  BOOST_REQUIRE_EQUAL(located, root->right()->left());
}

BOOST_AUTO_TEST_CASE(LocateNodeInTree_AC_0) {
  const auto root = AC_Tree_0();

  {
    const std::array<char, 1> data = {(char)0b11100000};
    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);
    BitExtractor extractor{input_stream};
    const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
    BOOST_REQUIRE_EQUAL(located, root->right()->right()->right()->left());
  }

  {
    const std::array<char, 1> data = {(char)0b11000000};
    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);
    BitExtractor extractor{input_stream};
    const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
    BOOST_REQUIRE_EQUAL(located, root->right()->right()->left()->left());
  }

  {
    const std::array<char, 1> data = {(char)0b10100000};
    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);
    BitExtractor extractor{input_stream};
    const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
    BOOST_REQUIRE_EQUAL(located, root->right()->left()->right());
  }

  {
    const std::array<char, 1> data = {(char)0b00000000};
    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);
    BitExtractor extractor{input_stream};
    const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
    BOOST_REQUIRE_EQUAL(located, root->left());
  }

  {
    const std::array<char, 1> data = {(char)0b00000000};
    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);
    BitExtractor extractor{input_stream};
    const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
    BOOST_REQUIRE_EQUAL(located, root->left());
  }

  {
    const std::array<char, 1> data = {(char)0b11110000};
    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);
    BitExtractor extractor{input_stream};
    const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
    BOOST_REQUIRE_EQUAL(located,
                      root->right()->right()->right()->right()->left());
  }

  {
    const std::array<char, 1> data = {(char)0b10000000};
    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);
    BitExtractor extractor{input_stream};
    const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
    BOOST_REQUIRE_EQUAL(located, root->right()->left()->left());
  }
}

BOOST_AUTO_TEST_CASE(LocateNodeInTree_DC_1) {
  const std::array<char, 1> data = {(char)0b10000000};

  ios::basic_array_source<char> input_source(data.data(), data.size());
  ios::stream<ios::basic_array_source<char>> input_stream(input_source);

  BitExtractor extractor{input_stream};

  const auto root = DC_Tree_1();
  const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
  BOOST_REQUIRE_EQUAL(located, root->right()->left());
}

BOOST_AUTO_TEST_CASE(LocateNodeInTree_AC_1) {
  const auto root = AC_Tree_1();

  {
    const std::array<char, 1> data = {(char)0b00000000};
    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);
    BitExtractor extractor{input_stream};
    const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
    BOOST_REQUIRE_EQUAL(located, root->left());
  }

  {
    const std::array<char, 1> data = {(char)0b10000000};
    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);
    BitExtractor extractor{input_stream};
    const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
    BOOST_REQUIRE_EQUAL(located, root->right()->left());
  }

  {
    const std::array<char, 1> data = {(char)0b11000000};
    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);
    BitExtractor extractor{input_stream};
    const auto located = SOSDecoder::LocateNodeInTree(extractor, root);
    BOOST_REQUIRE_EQUAL(located, root->right()->right()->left());
  }
}

BOOST_AUTO_TEST_CASE(ReadTable_Y1) {
  const auto AC_root = AC_Tree_0();
  const auto DC_root = DC_Tree_0();

  {
    const std::array<char, 5> data = {(char)0b10101110, (char)0b11100111,
                                      (char)0b01100001, (char)0b11110010,
                                      (char)0b0};
    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);

    BitExtractor extractor{input_stream};

    boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
    expected <<= 2, 0, 3, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, -1, -1, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

    const auto matrix = SOSDecoder::ReadMatrix(extractor, DC_root, AC_root);
    // prnt_matr( matrix );
    BOOST_REQUIRE_EQUAL(matrix, expected);
  }
}

BOOST_AUTO_TEST_CASE(ReadTable_Y2) {
  const auto AC_root = AC_Tree_0();
  const auto DC_root = DC_Tree_0();

  {
    const std::array<char, 3> data = {(char)0b00110111, (char)0b10101010,
                                      (char)0b01000000};

    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);

    BitExtractor extractor{input_stream};
    boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
    expected <<= -4, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

    const auto matrix = SOSDecoder::ReadMatrix(extractor, DC_root, AC_root);
    // prnt_matr( matrix );
    BOOST_REQUIRE_EQUAL(matrix, expected);
  }
}

BOOST_AUTO_TEST_CASE(ReadTable_Y3) {
  const auto AC_root = AC_Tree_0();
  const auto DC_root = DC_Tree_0();

  {
    const std::array<char, 4> data = {(char)0b01010000, (char)0b10101011,
                                      (char)0b10100000, (char)0b10000000};

    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);

    BitExtractor extractor{input_stream};

    boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
    expected <<= 5, -1, 1, 0, 0, 0, 0, 0, -1, -2, -1, 0, 0, 0, 0, 0, 0, -1, 0,
        0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

    const auto matrix = SOSDecoder::ReadMatrix(extractor, DC_root, AC_root);
    // prnt_matr( matrix );
    BOOST_REQUIRE_EQUAL(matrix, expected);
  }
}

BOOST_AUTO_TEST_CASE(ReadTable_Y4) {
  const auto AC_root = AC_Tree_0();
  const auto DC_root = DC_Tree_0();

  {
    const std::array<char, 4> data = {(char)0b00111100, (char)0b10000010,
                                      (char)0b11001000, (char)0b01000000};

    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);

    BitExtractor extractor{input_stream};

    boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
    expected <<= -4, 2, 2, 1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, -1, -1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

    const auto matrix = SOSDecoder::ReadMatrix(extractor, DC_root, AC_root);
    // prnt_matr( matrix );

    BOOST_REQUIRE_EQUAL(matrix, expected);
  }
}

BOOST_AUTO_TEST_CASE(ReadTable_Cb) {
  const auto AC_root = AC_Tree_1();
  const auto DC_root = DC_Tree_1();

  {
    const std::array<char, 2> data = {(char)0b10001011, (char)0b00000000};

    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);

    BitExtractor extractor{input_stream};

    boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
    expected <<= -1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    const auto matrix = SOSDecoder::ReadMatrix(extractor, DC_root, AC_root);
    // prnt_matr( matrix );
    BOOST_REQUIRE_EQUAL(matrix, expected);
  }
}

BOOST_AUTO_TEST_CASE(ReadTable_Cr) {
  const auto AC_root = AC_Tree_1();
  const auto DC_root = DC_Tree_1();
  {
    const std::array<char, 3> data = {(char)0b00111011, (char)0b10010111,
                                      (char)0b111};

    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);

    BitExtractor extractor{input_stream};

    boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
    expected <<= 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

    const auto matrix = SOSDecoder::ReadMatrix(extractor, DC_root, AC_root);
    // prnt_matr( matrix );
    BOOST_REQUIRE_EQUAL(matrix, expected);
  }
}

BOOST_AUTO_TEST_CASE(ReadMCU) {
  const auto AC_root_0 = AC_Tree_0();
  const auto DC_root_0 = DC_Tree_0();
  const auto AC_root_1 = AC_Tree_1();
  const auto DC_root_1 = DC_Tree_1();
  {
    const std::array<char, 19> data = {
        (char)0b10101110, (char)0b11100111, (char)0b01100001, (char)0b11110010,
        (char)0b00011011, (char)0b11010101, (char)0b00100010, (char)0b10000101,
        (char)0b01011101, (char)0b00000100, (char)0b00111100, (char)0b10000010,
        (char)0b11001000, (char)0b01001000, (char)0b10110001, (char)0b11011100,
        (char)0b10111111, (char)0b11111111, (char)0b11011001};

    ios::basic_array_source<char> input_source(data.data(), data.size());
    ios::stream<ios::basic_array_source<char>> input_stream(input_source);

    BitExtractor extractor{input_stream};

    DCTTable dct;
    dct.precision = 8;
    dct.width = 16;
    dct.height = 16;

    dct.components.resize(3);

    dct.components[0].id = 1;
    dct.components[0].h = 2;
    dct.components[0].v = 2;
    dct.components[0].dqtId = 0;

    dct.components[1].id = 2;
    dct.components[1].h = 1;
    dct.components[1].v = 1;
    dct.components[1].dqtId = 1;

    dct.components[2].id = 3;
    dct.components[2].h = 1;
    dct.components[2].v = 1;
    dct.components[2].dqtId = 1;

    std::vector<SOSDecoder::Channel> channels;
    channels.resize(3);
    channels[0].id = 1;
    channels[0].ac_id = 0;
    channels[0].dc_id = 0;

    channels[1].id = 2;
    channels[1].ac_id = 1;
    channels[1].dc_id = 1;

    channels[2].id = 3;
    channels[2].ac_id = 1;
    channels[2].dc_id = 1;

    std::vector<std::shared_ptr<HuffmanTree::Node>> AC_HuffmanTables{
        AC_Tree_0(), AC_Tree_1()};
    std::vector<std::shared_ptr<HuffmanTree::Node>> DC_HuffmanTables{
        DC_Tree_0(), DC_Tree_1()};

    const auto mcu = SOSDecoder::ReadMCU(extractor, dct, channels,
                                         AC_HuffmanTables, DC_HuffmanTables);

    boost::numeric::ublas::matrix<uint8_t> cs1_1(8, 8);
    cs1_1 <<= 2, 0, 3, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    boost::numeric::ublas::matrix<uint8_t> cs1_2(8, 8);
    cs1_2 <<= -2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    boost::numeric::ublas::matrix<uint8_t> cs1_3(8, 8);
    cs1_3 <<= 3, -1, 1, 0, 0, 0, 0, 0, -1, -2, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0,
        0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    boost::numeric::ublas::matrix<uint8_t> cs1_4(8, 8);
    cs1_4 <<= -1, 2, 2, 1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, -1, -1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

    printMatrix(mcu.Cs1.at(0));
    printMatrix(mcu.Cs1.at(1));
    printMatrix(mcu.Cs1.at(2));
    printMatrix(mcu.Cs1.at(3));

    BOOST_REQUIRE_EQUAL(mcu.Cs1.size(), 4);
    BOOST_REQUIRE_EQUAL(cs1_1, mcu.Cs1.at(0));
    BOOST_REQUIRE_EQUAL(cs1_2, mcu.Cs1.at(1));
    BOOST_REQUIRE_EQUAL(cs1_3, mcu.Cs1.at(2));
    BOOST_REQUIRE_EQUAL(cs1_4, mcu.Cs1.at(3));

    boost::numeric::ublas::matrix<uint8_t> cb(8, 8);
    cb <<= -1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    BOOST_REQUIRE_EQUAL(mcu.Cs2.size(), 1);
    BOOST_REQUIRE_EQUAL(cb, mcu.Cs2.at(0));

    boost::numeric::ublas::matrix<uint8_t> cr(8, 8);
    cr <<= 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    BOOST_REQUIRE_EQUAL(mcu.Cs3.size(), 1);
    BOOST_REQUIRE_EQUAL(cr, mcu.Cs3.at(0));
  }
}

BOOST_AUTO_TEST_CASE(QuantMCU) {
  SOSDecoder::Cs mcu;

  {
    boost::numeric::ublas::matrix<uint8_t> cs1_1(8, 8);
    cs1_1 <<= 2, 0, 3, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    boost::numeric::ublas::matrix<uint8_t> cs1_2(8, 8);
    cs1_2 <<= -2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    boost::numeric::ublas::matrix<uint8_t> cs1_3(8, 8);
    cs1_3 <<= 3, -1, 1, 0, 0, 0, 0, 0, -1, -2, -1, 0, 0, 0, 0, 0, 0, -1, 0, 0,
        0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    boost::numeric::ublas::matrix<uint8_t> cs1_4(8, 8);
    cs1_4 <<= -1, 2, 2, 1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, -1, -1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

    mcu.Cs1.push_back(cs1_1);
    mcu.Cs1.push_back(cs1_2);
    mcu.Cs1.push_back(cs1_3);
    mcu.Cs1.push_back(cs1_4);

    boost::numeric::ublas::matrix<uint8_t> cb(8, 8);
    cb <<= -1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

    mcu.Cs2.push_back(cb);

    boost::numeric::ublas::matrix<uint8_t> cr(8, 8);
    cr <<= 0, 0, 0, 0, 0, 0, 0, 0, 1, -1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
    mcu.Cs3.push_back(cr);
  }

  std::cout << "____0000" << std::endl;

  std::vector<DCTComponent> components;

  components.resize(3);

  components[0].id = 1;
  components[0].h = 2;
  components[0].v = 2;
  components[0].dqtId = 0;

  components[1].id = 2;
  components[1].h = 1;
  components[1].v = 1;
  components[1].dqtId = 1;

  components[2].id = 3;
  components[2].h = 1;
  components[2].v = 1;
  components[2].dqtId = 1;

  boost::numeric::ublas::matrix<uint8_t> quant1(8, 8);
  quant1 <<= 0xA0, 0x6E, 0x64, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF, 0x78, 0x78, 0x8C,
      0xBE, 0xFF, 0xFF, 0xFF, 0xFF, 0x8C, 0x82, 0xA0, 0xF0, 0xFF, 0xFF, 0xFF,
      0xFF, 0x8C, 0xAA, 0xDC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB4, 0xDC, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF;

  boost::numeric::ublas::matrix<char> quant2(8, 8);
  quant2 <<= 0xaa, 0xb4, 0xf0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xb4, 0xd2, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xf0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF;

  std::cout << "____1111" << std::endl;

  const auto mcu2 = SOSDecoder::QuantMCU(mcu, components, {quant1, quant2});

  std::cout << "____22222" << std::endl;

  boost::numeric::ublas::matrix<uint8_t> cs1_1(8, 8);
  cs1_1 <<= 320, 0, 300, 0, 0, 0, 0, 0, 0, 120, 280, 0, 0, 0, 0, 0, 0, -130,
      -160, 0, 0, 0, 0, 0, 140, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

  boost::numeric::ublas::matrix<uint8_t> cs1_2(8, 8);
  cs1_2 <<= -320, 110, 100, 160, 0, 0, 0, 0, 0, 0, 140, 0, 0, 0, 0, 0, 0, -130,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

  boost::numeric::ublas::matrix<uint8_t> cs1_3(8, 8);
  cs1_3 <<= 480, -110, 100, 0, 0, 0, 0, 0, -120, -240, -140, 0, 0, 0, 0, 0, 0,
      -130, 0, 0, 0, 0, 0, 0, -140, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

  boost::numeric::ublas::matrix<uint8_t> cs1_4(8, 8);
  cs1_4 <<= -160, 220, 200, 160, 0, 0, 0, 0, -120, 0, -140, 0, 0, 0, 0, 0, -140,
      -130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

  BOOST_REQUIRE_EQUAL(mcu2.Cs1.size(), 4);

  printMatrix(mcu2.Cs1.at(0));
  printMatrix(mcu2.Cs1.at(1));
  printMatrix(mcu2.Cs1.at(2));
  printMatrix(mcu2.Cs1.at(3));

  BOOST_REQUIRE_EQUAL(cs1_1, mcu2.Cs1.at(0));
  BOOST_REQUIRE_EQUAL(cs1_2, mcu2.Cs1.at(1));
  BOOST_REQUIRE_EQUAL(cs1_3, mcu2.Cs1.at(2));
  BOOST_REQUIRE_EQUAL(cs1_4, mcu2.Cs1.at(3));

  boost::numeric::ublas::matrix<uint8_t> cb(8, 8);
  cb <<= -170, 0, 0, 0, 0, 0, 0, 0, 180, 210, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;
  BOOST_REQUIRE_EQUAL(mcu2.Cs2.size(), 1);
  BOOST_REQUIRE_EQUAL(cb, mcu2.Cs2.at(0));

  boost::numeric::ublas::matrix<uint8_t> cr(8, 8);
  cr <<= 0, 0, 0, 0, 0, 0, 0, 0, 180, -210, 0, 0, 0, 0, 0, 0, 240, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

  BOOST_REQUIRE_EQUAL(mcu2.Cs3.size(), 1);
  BOOST_REQUIRE_EQUAL(cr, mcu2.Cs3.at(0));
}

BOOST_AUTO_TEST_CASE(ReverseDQT_Y1) {
  boost::numeric::ublas::matrix<int16_t> src(8, 8);
  src <<= 320, 0, 300, 0, 0, 0, 0, 0, 0, 120, 280, 0, 0, 0, 0, 0, 0, -130, -160,
      0, 0, 0, 0, 0, 140, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

  boost::numeric::ublas::matrix<int16_t> cs1_1(8, 8);
  cs1_1 <<= 138, 92, 27, -17, -17, 28, 93, 139, 136, 82, 5, -51, -55, -8, 61,
      111, 143, 80, -9, -77, -89, -41, 32, 86, 157, 95, 6, -62, -76, -33, 36,
      86, 147, 103, 37, -12, -21, 11, 62, 100, 87, 72, 50, 36, 37, 55, 79, 95,
      -10, 5, 31, 56, 71, 73, 68, 62, -87, -50, 6, 56, 79, 72, 48, 29;

  const auto cs = SOSDecoder::ReverseDQT_1(src);

  printMatrix(cs);
  printMatrix(cs1_1);

  BOOST_REQUIRE_EQUAL(cs1_1, cs);
}

BOOST_AUTO_TEST_CASE(ReverseDQT_cb) {
  boost::numeric::ublas::matrix<int8_t> src(8, 8);
  src <<= -170, 0, 0, 0, 0, 0, 0, 0, 180, 210, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0;

  boost::numeric::ublas::matrix<int8_t> cs1_1(8, 8);
  cs1_1 <<= 60, 52, 38, 20, 0, -18, -32, -40, 48, 41, 29, 13, -3, -19, -31, -37,
      25, 20, 12, 2, -9, -19, -27, -32, -4, -6, -9, -13, -17, -20, -23, -25,
      -37, -35, -33, -29, -25, -21, -18, -17, -67, -63, -55, -44, -33, -22, -14,
      -10, -90, -84, -71, -56, -39, -23, -11, -4, -102, -95, -81, -62, -42, -23,
      -9, -1;

  const auto cs = SOSDecoder::ReverseDQT3(src);

  printMatrix(cs);
  printMatrix(cs1_1);

  //  BOOST_REQUIRE_EQUAL(cs1_1, cs);
}

BOOST_AUTO_TEST_CASE(YCbCrToRGB) {

  const auto fix_tbl = [](auto &tbl) {
    for (int i = 0; i < tbl.size1(); i++) {
      for (int j = 0; j < tbl.size2(); j++) {
        tbl(i, j) = std::min(std::max(0, tbl(i, j) + 128), 255);
      }
    }
  };

  boost::numeric::ublas::matrix<int16_t> y(8, 8);
  y <<= 138, 92, 27, -17, -17, 28, 93, 139, 136, 82, 5, -51, -55, -8, 61, 111,
      143, 80, -9, -77, -89, -41, 32, 86, 157, 95, 6, -62, -76, -33, 36, 86,
      147, 103, 37, -12, -21, 11, 62, 100, 87, 72, 50, 36, 37, 55, 79, 95, -10,
      5, 31, 56, 71, 73, 68, 62, -87, -50, 6, 56, 79, 72, 48, 29;
  fix_tbl(y);

  boost::numeric::ublas::matrix<int16_t> cb(8, 8);
  cb <<= 60, 52, 38, 20, 0, -18, -32, -40, 48, 41, 29, 13, -3, -19, -31, -37,
      25, 20, 12, 2, -9, -19, -27, -32, -4, -6, -9, -13, -17, -20, -23, -25,
      -37, -35, -33, -29, -25, -21, -18, -17, -67, -63, -55, -44, -33, -22, -14,
      -10, -90, -84, -71, -56, -39, -23, -11, -4, -102, -95, -81, -62, -42, -23,
      -9, -1;
  fix_tbl(cb);

  boost::numeric::ublas::matrix<int16_t> cr(8, 8);
  cr <<= 19, 27, 41, 60, 80, 99, 113, 120, 0, 6, 18, 34, 51, 66, 78, 85, -27,
      -22, -14, -4, 7, 17, 25, 30, -43, -41, -38, -34, -30, -27, -24, -22, -35,
      -36, -39, -43, -47, -51, -53, -55, -5, -9, -17, -28, -39, -50, -58, -62,
      32, 26, 14, -1, -18, -34, -46, -53, 58, 50, 36, 18, -2, -20, -34, -42;
  fix_tbl(cr);

  printMatrix(y);
  printMatrix(cb);
  printMatrix(cr);

  const auto res = SOSDecoder::YCbCrToRGB(y, cb, cr);

  printMatrix(std::get<0>(res));
  printMatrix(std::get<1>(res));
  printMatrix(std::get<2>(res));

  //    BOOST_REQUIRE_EQUAL(cs1_1, cs);
}

// BOOST_AUTO_TEST_CASE(Invoke) {
//    const std::array<char, 19> data = {
//        (char)0b10101110,
//        (char)0b11100111,
//        (char)0b01100001,
//        (char)0b11110010,
//        (char)0b00011011,
//        (char)0b11010101,
//        (char)0b00100010,
//        (char)0b10000101,
//        (char)0b01011101,
//        (char)0b00000100,
//        (char)0b00111100,
//        (char)0b10000010,
//        (char)0b11001000,
//        (char)0b01001000,
//        (char)0b10110001,
//        (char)0b11011100,
//        (char)0b10111111,
//        (char)0b11111111,
//        (char)0b11011001
//    };

//    basic_array_source<char> input_source(data.data(), data.size());
//    stream<basic_array_source<char>> input_stream(input_source);

//    Context context;
//    SOSDecoder decoder;

//    decoder.Invoke(input_stream, context);
//}

BOOST_AUTO_TEST_SUITE_END()
