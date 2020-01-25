#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <fstream>
#include <iostream>
using namespace boost::iostreams;
using namespace std;

#include "dht_decoder.hpp"

BOOST_AUTO_TEST_SUITE(DHTDecoderTests)

BOOST_AUTO_TEST_CASE(PushHuffValue_DC_0) {
    const auto root = std::make_shared<DHTNode>();

    const auto node_0_1 = PushHuffValue(root, 0, 0x03);
    BOOST_REQUIRE(root->left == node_0_1);
    const auto node_1_1 = PushHuffValue(root, 1, 0x02);
    BOOST_REQUIRE(root->right->left == node_1_1);
}

BOOST_AUTO_TEST_CASE(PushHuffValue_DC_1) {
    const auto root = std::make_shared<DHTNode>();

    const auto node_0_1 = PushHuffValue(root, 0, 0x00);
    BOOST_REQUIRE(root->left == node_0_1);
    const auto node_1_1 = PushHuffValue(root, 1, 0x01);
    BOOST_REQUIRE(root->right->left == node_1_1);
}

BOOST_AUTO_TEST_CASE(PushHuffValue_AC_0) {
    const auto root = std::make_shared<DHTNode>();

    const auto node_0_1 = PushHuffValue(root, 0, 0x01);
    BOOST_REQUIRE(root->left == node_0_1);

    const auto node_2_1 = PushHuffValue(root, 2, 0x00);
    const auto node_2_2 = PushHuffValue(root, 2, 0x12);
    BOOST_REQUIRE(root->right->left->left == node_2_1);
    BOOST_REQUIRE(root->right->left->right == node_2_2);

    const auto node_3_1 = PushHuffValue(root, 3, 0x02);
    const auto node_3_2 = PushHuffValue(root, 3, 0x11);
    const auto node_3_3 = PushHuffValue(root, 3, 0x31);

    BOOST_REQUIRE(root->right->right->left->left == node_3_1);
    BOOST_REQUIRE(root->right->right->left->right == node_3_2);
    BOOST_REQUIRE(root->right->right->right->left == node_3_3);

    const auto node_4_31= PushHuffValue(root, 4, 0x21);
    BOOST_REQUIRE(root->right->right->right->right->left == node_4_31);
}

BOOST_AUTO_TEST_CASE(PushHuffValue_AC_1) {
    const auto root = std::make_shared<DHTNode>();

    const auto node_0_1 = PushHuffValue(root, 0, 0x11 );
    BOOST_REQUIRE(root->left == node_0_1);
    const auto node_1_1 = PushHuffValue(root, 1, 0x00);
    BOOST_REQUIRE(root->right->left == node_1_1);
    const auto node_3_1 = PushHuffValue(root, 2, 0x01);
    BOOST_REQUIRE(root->right->right->left == node_3_1);
}

BOOST_AUTO_TEST_SUITE_END()
