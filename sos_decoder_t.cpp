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

//// TODO: в общий модуль
//namespace boost::numeric::ublas {

//bool operator==(const boost::numeric::ublas::matrix<char> &left, const boost::numeric::ublas::matrix<char> &right) {
//    using size_type = boost::numeric::ublas::matrix<char>::size_type;

//    if (left.size1() != right.size1()) {
//        return false;
//    }

//    if (left.size2() != right.size2()){
//        return false;
//    }

//    for(size_type i=0;i< left.size1();++i)
//    {
//        for (size_type j=0;j<left.size2();++j)
//        {
//            if (left(i,j) != right(i,j)) {
//                return false;
//            }
//        }
//    }

//    return true;
//}

//}

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

    std::shared_ptr<DHTNode> root = std::make_shared<DHTNode>();
    root->left = std::make_shared<DHTNode>();
    root->left->data = 0x03;

    root->right = std::make_shared<DHTNode>();
    root->right->left = std::make_shared<DHTNode>();
    root->right->left->data = 0x02;

    const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
    BOOST_CHECK_EQUAL(located, root->right->left);
}

BOOST_AUTO_TEST_CASE(LocateNodeInTree_AC_0) {
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

    std::shared_ptr<DHTNode> root = std::make_shared<DHTNode>();
    root->left = std::make_shared<DHTNode>();
    root->left->data = 0x00;

    root->right = std::make_shared<DHTNode>();
    root->right->left = std::make_shared<DHTNode>();
    root->right->left->data = 0x01;

    const auto located = SOSDecoder::LocateNodeInTree( extractor, root );
    BOOST_CHECK_EQUAL(located, root->right->left);
}

BOOST_AUTO_TEST_CASE(LocateNodeInTree_AC_1) {
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
#ifdef NOT_WORKING
    boost::numeric::ublas::matrix<uint8_t> expected(8, 8);
    expected <<=    2,  0,  3, 0, 0, 0, 0, 0,
                    0,  1,  2, 0, 0, 0, 0, 0,
                    0, -1, -1, 0, 0, 0, 0, 0,
                    1,  0,  0, 0, 0, 0, 0, 0,
                    0,  0,  0, 0, 0, 0, 0, 0,
                    0,  0,  0, 0, 0, 0, 0, 0,
                    0,  0,  0, 0, 0, 0, 0, 0,
                    0,  0,  0, 0, 0, 0, 0, 0;
    //     0

    const std::array<char, 5> data = {
        (char)0b10101110, (char)0b11100111, 0b01100001, (char)0b11110010, 0b0
    };

    basic_array_source<char> input_source(data.data(), data.size());
    stream<basic_array_source<char>> input_stream(input_source);

    SOSDecoder::BitExtractor extractor{ input_stream };

    //-------------------------------------
    std::shared_ptr<DHTNode> AC_HuffmanTables = {

    };
    std::shared_ptr<DHTNode> DC_HuffmanTables = {

    };

    //-------------------------------------
    const auto matrix = SOSDecoder::ReadMatrix( extractor, AC_HuffmanTables, DC_HuffmanTables );
    BOOST_CHECK_EQUAL(expected, matrix);s
#endif
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
