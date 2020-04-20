#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <fstream>
#include <iostream>
#include <array>

using namespace boost::iostreams;
using namespace std;

#include "sos_decoder.hpp"
#include "utility.hpp"

//-------------------------------------
std::shared_ptr<DHTNode> DC_Tree_0() {
    std::shared_ptr<DHTNode> root = std::make_shared<DHTNode>();
    root->left = std::make_shared<DHTNode>();
    root->left->data = 0x03;

    root->right = std::make_shared<DHTNode>();
    root->right->left = std::make_shared<DHTNode>();
    root->right->left->data = 0x02;

    return root;
}

std::shared_ptr<DHTNode> DC_Tree_1() {
    std::shared_ptr<DHTNode> root = std::make_shared<DHTNode>();
    root->left = std::make_shared<DHTNode>();
    root->left->data = 0x00;

    root->right = std::make_shared<DHTNode>();
    root->right->left = std::make_shared<DHTNode>();
    root->right->left->data = 0x01;

    return root;
}

std::shared_ptr<DHTNode> AC_Tree_0() {
    const auto root = std::make_shared<DHTNode>();

//  const auto node_0_1 = PushHuffValue(root, 0, 0x01);
//  BOOST_REQUIRE(root->left == node_0_1);
    root->left = std::make_shared<DHTNode>();
    root->left->data = 0x01;

//  const auto node_2_1 = PushHuffValue(root, 2, 0x00);
//  const auto node_2_2 = PushHuffValue(root, 2, 0x12);
//  BOOST_REQUIRE(root->right->left->left == node_2_1);
//  BOOST_REQUIRE(root->right->left->right == node_2_2);
    root->right = std::make_shared<DHTNode>();
    root->right->left = std::make_shared<DHTNode>();
    root->right->left->left = std::make_shared<DHTNode>();
    root->right->left->left->data = 0x00;
    root->right->left->right = std::make_shared<DHTNode>();
    root->right->left->right->data = 0x12;

//  const auto node_3_1 = PushHuffValue(root, 3, 0x02);
//  const auto node_3_2 = PushHuffValue(root, 3, 0x11);
//  const auto node_3_3 = PushHuffValue(root, 3, 0x31);
//  BOOST_REQUIRE(root->right->right->left->left == node_3_1);
//  BOOST_REQUIRE(root->right->right->left->right == node_3_2);
//  BOOST_REQUIRE(root->right->right->right->left == node_3_3);
    root->right->right = std::make_shared<DHTNode>();
    root->right->right->left = std::make_shared<DHTNode>();
    root->right->right->left->left = std::make_shared<DHTNode>();
    root->right->right->left->left->data = 0x02;
    root->right->right->left->right = std::make_shared<DHTNode>();
    root->right->right->left->right->data = 0x11;
    root->right->right->right = std::make_shared<DHTNode>();
    root->right->right->right->left = std::make_shared<DHTNode>();
    root->right->right->right->left->data = 0x31;

//  const auto node_4_31= PushHuffValue(root, 4, 0x21);
//  BOOST_REQUIRE(root->right->right->right->right->left == node_4_31);
    root->right->right->right->right = std::make_shared<DHTNode>();
    root->right->right->right->right->left = std::make_shared<DHTNode>();
    root->right->right->right->right->left->data = 0x21;

    return root;
}

std::shared_ptr<DHTNode> AC_Tree_1() {
    const auto root = std::make_shared<DHTNode>();

//  const auto node_0_1 = PushHuffValue(root, 0, 0x11 );
//  BOOST_REQUIRE(root->left == node_0_1);
    root->left = std::make_shared<DHTNode>();
    root->left->data = 0x11;

//  const auto node_1_1 = PushHuffValue(root, 1, 0x00);
//  BOOST_REQUIRE(root->right->left == node_1_1);
    root->right = std::make_shared<DHTNode>();
    root->right->left = std::make_shared<DHTNode>();
    root->right->left->data = 0x00;

//  const auto node_3_1 = PushHuffValue(root, 2, 0x01);
//  BOOST_REQUIRE(root->right->right->left == node_3_1);
    root->right->right = std::make_shared<DHTNode>();
    root->right->right->left = std::make_shared<DHTNode>();
    root->right->right->left->data = 0x01;

    return root;
}

void prnt_matr( const boost::numeric::ublas::matrix<uint8_t>& matrix ) {
    std::cout << std::endl << "prnt_matr" << std::endl;

    for ( int i = 0; i < 8; i++ ) {
        for ( int j = 0; j < 8; j++ ) {
            std::cout << (int) matrix( i, j ) << '\t';
        }
        std::cout << std::endl;
    }

}

//-------------------------------------
BOOST_AUTO_TEST_SUITE(SOSDecoderTests)

BOOST_AUTO_TEST_CASE(BitExtractor) {
    const std::array<char, 4> data = {
        (char)0b10101001, 0b01111011, 0b01100101, 0b00000001
    };

    basic_array_source<char> input_source(data.data(), data.size());
    stream<basic_array_source<char>> input_stream(input_source);

    SOSDecoder::BitExtractor extractor{ input_stream };

    // 0b10101001
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);

    // 0b01111011
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 1);

    // 0b01100101
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);

    // 0b00000001
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 0);
    BOOST_REQUIRE(extractor.nextNumber() == 1);
}

BOOST_AUTO_TEST_CASE(BitExtractor_WithLength) {
    const std::array<char, 4> data = {
        (char)0b10101001, 0b01111011, 0b01100101, 0b00000001
    };

    basic_array_source<char> input_source(data.data(), data.size());
    stream<basic_array_source<char>> input_stream(input_source);

    SOSDecoder::BitExtractor extractor{ input_stream };

    //-------------------------------------
    BOOST_REQUIRE(extractor.nextNumber( 6 ) == 0b101010);
    BOOST_REQUIRE(extractor.nextNumber( 7 ) == 0b0101111);
    BOOST_REQUIRE(extractor.nextNumber( 8 ) == 0b01101100);
    BOOST_REQUIRE(extractor.nextNumber( 5 ) == 0b10100);
    BOOST_REQUIRE(extractor.nextNumber( 2 ) == 0b00);
    BOOST_REQUIRE(extractor.nextNumber( 4 ) == 0b0001);
}

BOOST_AUTO_TEST_CASE(LocateNodeInTree_DC_0) {
    const std::array<char, 1> data = { (char)0b10000000 };

    basic_array_source<char> input_source(data.data(), data.size());
    stream<basic_array_source<char>> input_stream(input_source);

    SOSDecoder::BitExtractor extractor{ input_stream };

    const auto root = DC_Tree_0();

    const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
    BOOST_CHECK_EQUAL(located, root->right->left);
}

BOOST_AUTO_TEST_CASE(LocateNodeInTree_AC_0) {
    const auto root = AC_Tree_0();

    {
        const std::array<char, 1> data = { (char)0b11100000 };
        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);
        SOSDecoder::BitExtractor extractor{ input_stream };
        const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
        BOOST_CHECK_EQUAL(located, root->right->right->right->left);
    }

    {
        const std::array<char, 1> data = { (char)0b11000000 };
        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);
        SOSDecoder::BitExtractor extractor{ input_stream };
        const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
        BOOST_CHECK_EQUAL(located, root->right->right->left->left);
    }

    {
        const std::array<char, 1> data = { (char)0b10100000 };
        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);
        SOSDecoder::BitExtractor extractor{ input_stream };
        const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
        BOOST_CHECK_EQUAL(located, root->right->left->right);
    }

    {
        const std::array<char, 1> data = { (char)0b00000000 };
        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);
        SOSDecoder::BitExtractor extractor{ input_stream };
        const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
        BOOST_CHECK_EQUAL(located, root->left);
    }

    {
        const std::array<char, 1> data = { (char)0b00000000 };
        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);
        SOSDecoder::BitExtractor extractor{ input_stream };
        const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
        BOOST_CHECK_EQUAL(located, root->left);
    }

    {
        const std::array<char, 1> data = { (char)0b11110000 };
        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);
        SOSDecoder::BitExtractor extractor{ input_stream };
        const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
        BOOST_CHECK_EQUAL(located, root->right->right->right->right->left);
    }

    {
        const std::array<char, 1> data = { (char)0b10000000 };
        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);
        SOSDecoder::BitExtractor extractor{ input_stream };
        const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
        BOOST_CHECK_EQUAL(located, root->right->left->left);
    }
}

BOOST_AUTO_TEST_CASE(LocateNodeInTree_DC_1) {
    const std::array<char, 1> data = { (char)0b10000000 };

    basic_array_source<char> input_source(data.data(), data.size());
    stream<basic_array_source<char>> input_stream(input_source);

    SOSDecoder::BitExtractor extractor{ input_stream };

    const auto root = DC_Tree_1();
    const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
    BOOST_CHECK_EQUAL(located, root->right->left);
}

BOOST_AUTO_TEST_CASE(LocateNodeInTree_AC_1) {
    const auto root = AC_Tree_1();

    {
        const std::array<char, 1> data = { (char)0b00000000 };
        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);
        SOSDecoder::BitExtractor extractor{ input_stream };
        const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
        BOOST_CHECK_EQUAL(located, root->left);
    }

    {
        const std::array<char, 1> data = { (char)0b10000000 };
        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);
        SOSDecoder::BitExtractor extractor{ input_stream };
        const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
        BOOST_CHECK_EQUAL(located, root->right->left);
    }

    {
        const std::array<char, 1> data = { (char)0b11000000 };
        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);
        SOSDecoder::BitExtractor extractor{ input_stream };
        const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
        BOOST_CHECK_EQUAL(located, root->right->right->left);
    }
}

BOOST_AUTO_TEST_CASE(ReadTable_Y1) {
    const auto AC_root = AC_Tree_0();
    const auto DC_root = DC_Tree_0();

    {
        const std::array<char, 5> data = {
              (char)0b10101110
            , (char)0b11100111
            , (char)0b01100001
            , (char)0b11110010
            , (char)0b0
        };
        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);

        SOSDecoder::BitExtractor extractor{ input_stream };

        boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
        expected <<=        2,  0,  3, 0, 0, 0, 0, 0,
                            0,  1,  2, 0, 0, 0, 0, 0,
                            0, -1, -1, 0, 0, 0, 0, 0,
                            1,  0,  0, 0, 0, 0, 0, 0,
                            0,  0,  0, 0, 0, 0, 0, 0,
                            0,  0,  0, 0, 0, 0, 0, 0,
                            0,  0,  0, 0, 0, 0, 0, 0,
                            0,  0,  0, 0, 0, 0, 0, 0;

        const auto matrix = SOSDecoder::ReadMatrix(
            extractor,
            DC_root,
            AC_root
        );
        // prnt_matr( matrix );
        BOOST_CHECK_EQUAL(matrix, expected);
    }
}

BOOST_AUTO_TEST_CASE(ReadTable_Y2) {
    const auto AC_root = AC_Tree_0();
    const auto DC_root = DC_Tree_0();

    {
        const std::array<char, 3> data = {
              (char)0b00110111
            , (char)0b10101010
            , (char)0b01000000
        };

        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);

        SOSDecoder::BitExtractor extractor{ input_stream };
        boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
        expected <<=    -4,  1, 1, 1, 0, 0, 0, 0,
                         0,  0, 1, 0, 0, 0, 0, 0,
                         0, -1, 0, 0, 0, 0, 0, 0,
                         0,  0, 0, 0, 0, 0, 0, 0,
                         0,  0, 0, 0, 0, 0, 0, 0,
                         0,  0, 0, 0, 0, 0, 0, 0,
                         0,  0, 0, 0, 0, 0, 0, 0,
                         0,  0, 0, 0, 0, 0, 0, 0;

        const auto matrix = SOSDecoder::ReadMatrix(
            extractor,
            DC_root,
            AC_root
        );
        // prnt_matr( matrix );
        BOOST_CHECK_EQUAL(matrix, expected);
    }
}

BOOST_AUTO_TEST_CASE(ReadTable_Y3) {
    const auto AC_root = AC_Tree_0();
    const auto DC_root = DC_Tree_0();

    {
        const std::array<char, 4> data = {
            (char)0b01010000,
            (char)0b10101011,
            (char)0b10100000,
            (char)0b10000000
        };

        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);

        SOSDecoder::BitExtractor extractor{ input_stream };

        boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
        expected <<=
                5, -1,  1, 0, 0, 0, 0, 0,
               -1, -2, -1, 0, 0, 0, 0, 0,
                0, -1,  0, 0, 0, 0, 0, 0,
               -1,  0,  0, 0, 0, 0, 0, 0,
                0,  0,  0, 0, 0, 0, 0, 0,
                0,  0,  0, 0, 0, 0, 0, 0,
                0,  0,  0, 0, 0, 0, 0, 0,
                0,  0,  0, 0, 0, 0, 0, 0;

        const auto matrix = SOSDecoder::ReadMatrix(
            extractor,
            DC_root,
            AC_root
        );
        // prnt_matr( matrix );
        BOOST_CHECK_EQUAL(matrix, expected);
    }
}

BOOST_AUTO_TEST_CASE(ReadTable_Y4) {
    const auto AC_root = AC_Tree_0();
    const auto DC_root = DC_Tree_0();

    {
        const std::array<char, 4> data = {
            (char)0b00111100,
            (char)0b10000010,
            (char)0b11001000,
            (char)0b01000000
        };

        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);

        SOSDecoder::BitExtractor extractor{ input_stream };

        boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
        expected <<=
                -4,  2,  2, 1, 0, 0, 0, 0,
                -1,  0, -1, 0, 0, 0, 0, 0,
                -1, -1,  0, 0, 0, 0, 0, 0,
                 0,  0,  0, 0, 0, 0, 0, 0,
                 0,  0,  0, 0, 0, 0, 0, 0,
                 0,  0,  0, 0, 0, 0, 0, 0,
                 0,  0,  0, 0, 0, 0, 0, 0,
                 0,  0,  0, 0, 0, 0, 0, 0;

        const auto matrix = SOSDecoder::ReadMatrix(
            extractor,
            DC_root,
            AC_root
        );
        // prnt_matr( matrix );

        BOOST_CHECK_EQUAL(matrix, expected);
    }
}

BOOST_AUTO_TEST_CASE(ReadTable_Cb) {
    const auto AC_root = AC_Tree_1();
    const auto DC_root = DC_Tree_1();

    {
        const std::array<char, 2> data = {
            (char)0b10001011,
            (char)0b00000000
        };

        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);

        SOSDecoder::BitExtractor extractor{ input_stream };

        boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
        expected <<=
                -1, 0, 0, 0, 0, 0, 0, 0,
                1, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0;
        const auto matrix = SOSDecoder::ReadMatrix(
            extractor,
            DC_root,
            AC_root
        );
        // prnt_matr( matrix );
        BOOST_CHECK_EQUAL(matrix, expected);
    }
}

BOOST_AUTO_TEST_CASE(ReadTable_Cr) {
    const auto AC_root = AC_Tree_1();
    const auto DC_root = DC_Tree_1();
    {
        const std::array<char, 3> data = {
            (char)0b00111011,
            (char)0b10010111,
            (char)0b111
        };

        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);

        SOSDecoder::BitExtractor extractor{ input_stream };

        boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
        expected <<=
            0,  0, 0, 0, 0, 0, 0, 0,
            1, -1, 0, 0, 0, 0, 0, 0,
            1,  0, 0, 0, 0, 0, 0, 0,
            0,  0, 0, 0, 0, 0, 0, 0,
            0,  0, 0, 0, 0, 0, 0, 0,
            0,  0, 0, 0, 0, 0, 0, 0,
            0,  0, 0, 0, 0, 0, 0, 0,
            0,  0, 0, 0, 0, 0, 0, 0;

        const auto matrix = SOSDecoder::ReadMatrix(
            extractor,
            DC_root,
            AC_root
        );
        // prnt_matr( matrix );
        BOOST_CHECK_EQUAL(matrix, expected);
    }
}

BOOST_AUTO_TEST_CASE(ReadMCU) {
    const auto AC_root_0 = AC_Tree_0();
    const auto DC_root_0 = DC_Tree_0();
    const auto AC_root_1 = AC_Tree_1();
    const auto DC_root_1 = DC_Tree_1();
    {
        const std::array<char, 19> data = {
            (char)0b10101110,
            (char)0b11100111,
            (char)0b01100001,
            (char)0b11110010,
            (char)0b00011011,
            (char)0b11010101,
            (char)0b00100010,
            (char)0b10000101,
            (char)0b01011101,
            (char)0b00000100,
            (char)0b00111100,
            (char)0b10000010,
            (char)0b11001000,
            (char)0b01001000,
            (char)0b10110001,
            (char)0b11011100,
            (char)0b10111111,
            (char)0b11111111,
            (char)0b11011001
        };

        basic_array_source<char> input_source(data.data(), data.size());
        stream<basic_array_source<char>> input_stream(input_source);

        SOSDecoder::BitExtractor extractor{ input_stream };

        DCTTable dct;
        dct.precision = 8;
        dct.width = 16;
        dct.height = 16;

        dct.components.resize( 3 );

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
        channels[0].id  = 1;
        channels[0].ac_id = 0;
        channels[0].dc_id = 0;

        channels[1].id  = 2;
        channels[1].ac_id = 1;
        channels[1].dc_id = 1;

        channels[2].id  = 3;
        channels[2].ac_id = 1;
        channels[2].dc_id = 1;

        std::vector<std::shared_ptr<DHTNode>> AC_HuffmanTables{
            AC_Tree_0(), AC_Tree_1()
        };
        std::vector<std::shared_ptr<DHTNode>> DC_HuffmanTables{
            DC_Tree_0(), DC_Tree_1()
        };

        const auto mcu = SOSDecoder::ReadMCU(
            extractor,
            dct,
            channels,
            AC_HuffmanTables,
            DC_HuffmanTables
        );

        boost::numeric::ublas::matrix<uint8_t> cs1_1(8, 8);
        cs1_1 <<=       2,  0,  3, 0, 0, 0, 0, 0,
                        0,  1,  2, 0, 0, 0, 0, 0,
                        0, -1, -1, 0, 0, 0, 0, 0,
                        1,  0,  0, 0, 0, 0, 0, 0,
                        0,  0,  0, 0, 0, 0, 0, 0,
                        0,  0,  0, 0, 0, 0, 0, 0,
                        0,  0,  0, 0, 0, 0, 0, 0,
                        0,  0,  0, 0, 0, 0, 0, 0;
        boost::numeric::ublas::matrix<uint8_t> cs1_2(8, 8);
        cs1_2 <<=
            -2,  1, 1, 1, 0, 0, 0, 0,
             0,  0, 1, 0, 0, 0, 0, 0,
             0, -1, 0, 0, 0, 0, 0, 0,
             0,  0, 0, 0, 0, 0, 0, 0,
             0,  0, 0, 0, 0, 0, 0, 0,
             0,  0, 0, 0, 0, 0, 0, 0,
             0,  0, 0, 0, 0, 0, 0, 0,
             0,  0, 0, 0, 0, 0, 0, 0;
        boost::numeric::ublas::matrix<uint8_t> cs1_3(8, 8);
        cs1_3 <<=
            3, -1,  1, 0, 0, 0, 0, 0,
           -1, -2, -1, 0, 0, 0, 0, 0,
            0, -1,  0, 0, 0, 0, 0, 0,
           -1,  0,  0, 0, 0, 0, 0, 0,
            0,  0,  0, 0, 0, 0, 0, 0,
            0,  0,  0, 0, 0, 0, 0, 0,
            0,  0,  0, 0, 0, 0, 0, 0,
            0,  0,  0, 0, 0, 0, 0, 0;
        boost::numeric::ublas::matrix<uint8_t> cs1_4(8, 8);
        cs1_4 <<=
            -1,  2,  2, 1, 0, 0, 0, 0,
            -1,  0, -1, 0, 0, 0, 0, 0,
            -1, -1,  0, 0, 0, 0, 0, 0,
             0,  0,  0, 0, 0, 0, 0, 0,
             0,  0,  0, 0, 0, 0, 0, 0,
             0,  0,  0, 0, 0, 0, 0, 0,
             0,  0,  0, 0, 0, 0, 0, 0,
             0,  0,  0, 0, 0, 0, 0, 0;

        prnt_matr( mcu.Cs1.at(0) );
        prnt_matr( mcu.Cs1.at(1) );
        prnt_matr( mcu.Cs1.at(2) );
        prnt_matr( mcu.Cs1.at(3) );

        BOOST_CHECK_EQUAL(mcu.Cs1.size(), 4);
        BOOST_CHECK_EQUAL(cs1_1, mcu.Cs1.at(0));
        BOOST_CHECK_EQUAL(cs1_2, mcu.Cs1.at(1));
        BOOST_CHECK_EQUAL(cs1_3, mcu.Cs1.at(2));
        BOOST_CHECK_EQUAL(cs1_4, mcu.Cs1.at(3));

        boost::numeric::ublas::matrix<uint8_t> cb(8, 8);
        cb <<=
                -1, 0, 0, 0, 0, 0, 0, 0,
                1, 1, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0;
        BOOST_CHECK_EQUAL(mcu.Cs2.size(), 1);
        BOOST_CHECK_EQUAL(cb, mcu.Cs2.at(0));

        boost::numeric::ublas::matrix<uint8_t> cr(8, 8);
        cr <<=
            0,  0, 0, 0, 0, 0, 0, 0,
            1, -1, 0, 0, 0, 0, 0, 0,
            1,  0, 0, 0, 0, 0, 0, 0,
            0,  0, 0, 0, 0, 0, 0, 0,
            0,  0, 0, 0, 0, 0, 0, 0,
            0,  0, 0, 0, 0, 0, 0, 0,
            0,  0, 0, 0, 0, 0, 0, 0,
            0,  0, 0, 0, 0, 0, 0, 0;
        BOOST_CHECK_EQUAL(mcu.Cs3.size(), 1);
        BOOST_CHECK_EQUAL(cr, mcu.Cs3.at(0));
    }
}

BOOST_AUTO_TEST_CASE(Invoke) {
//    const std::array<char, 31> data = {
//        (char)0x00, (char)0x0c, (char)0x03, (char)0x01, (char)0x00, (char)0x02,
//        (char)0x11, (char)0x03, (char)0x11, (char)0x00, (char)0x3f, (char)0x00,
//        (char)0xae, (char)0xe7, (char)0x61, (char)0xf2, (char)0x1b, (char)0xd5,
//        (char)0x22, (char)0x85, (char)0x5d, (char)0x04, (char)0x3c, (char)0x82,
//        (char)0xc8, (char)0x48, (char)0xb1, (char)0xdc, (char)0xbf, (char)0xff,
//        (char)0xd9
//     };

//    basic_array_source<char> input_source(data.data(), data.size());
//    stream<basic_array_source<char>> input_stream(input_source);

//    Context context;
//    SOSDecoder decoder;

//    decoder.Invoke(input_stream, context);
}

BOOST_AUTO_TEST_SUITE_END()
