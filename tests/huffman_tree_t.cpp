#include <boost/test/unit_test.hpp>

#include "huffman_tree.hpp"

BOOST_AUTO_TEST_SUITE(HuffmanTreeTests)

namespace {
auto createRoot() { return std::make_shared<HuffmanTree::Node>(nullptr); }
} // namespace

BOOST_AUTO_TEST_CASE(createAndInsertNode_DC_0) {
  const auto Root = createRoot();
  const auto Node_0_1 = HuffmanTree::createAndInsertNode(Root, 0, 0x03);
  BOOST_REQUIRE_EQUAL(Root->left(), Node_0_1);
  const auto Node_1_1 = HuffmanTree::createAndInsertNode(Root, 1, 0x02);
  BOOST_REQUIRE_EQUAL(Root->right()->left(), Node_1_1);
}

BOOST_AUTO_TEST_CASE(createAndInsertNode_DC_1) {
  const auto Root = createRoot();
  const auto Node_0_1 = HuffmanTree::createAndInsertNode(Root, 0, 0x00);
  BOOST_REQUIRE_EQUAL(Root->left(), Node_0_1);
  const auto Node_1_1 = HuffmanTree::createAndInsertNode(Root, 1, 0x01);
  BOOST_REQUIRE_EQUAL(Root->right()->left(), Node_1_1);
}

BOOST_AUTO_TEST_CASE(createAndInsertNode_AC_0) {
  const auto Root = createRoot();
  const auto Node_0_1 = HuffmanTree::createAndInsertNode(Root, 0, 0x01);
  BOOST_REQUIRE_EQUAL(Root->left(), Node_0_1);

  const auto Node_2_1 = HuffmanTree::createAndInsertNode(Root, 2, 0x00);
  const auto Node_2_2 = HuffmanTree::createAndInsertNode(Root, 2, 0x12);
  BOOST_REQUIRE_EQUAL(Root->right()->left()->left(), Node_2_1);
  BOOST_REQUIRE_EQUAL(Root->right()->left()->right(), Node_2_2);

  const auto Node_3_1 = HuffmanTree::createAndInsertNode(Root, 3, 0x02);
  const auto Node_3_2 = HuffmanTree::createAndInsertNode(Root, 3, 0x11);
  const auto Node_3_3 = HuffmanTree::createAndInsertNode(Root, 3, 0x31);

  BOOST_REQUIRE_EQUAL(Root->right()->right()->left()->left(), Node_3_1);
  BOOST_REQUIRE_EQUAL(Root->right()->right()->left()->right(), Node_3_2);
  BOOST_REQUIRE_EQUAL(Root->right()->right()->right()->left(), Node_3_3);

  const auto Node_4_31 = HuffmanTree::createAndInsertNode(Root, 4, 0x21);
  BOOST_REQUIRE_EQUAL(Root->right()->right()->right()->right()->left(),
                      Node_4_31);
}

BOOST_AUTO_TEST_CASE(createAndInsertNode_AC_1) {
  const auto Root = createRoot();
  const auto Node_0_1 = HuffmanTree::createAndInsertNode(Root, 0, 0x11);
  BOOST_REQUIRE_EQUAL(Root->left(), Node_0_1);
  const auto Node_1_1 = HuffmanTree::createAndInsertNode(Root, 1, 0x00);
  BOOST_REQUIRE_EQUAL(Root->right()->left(), Node_1_1);
  const auto Node_3_1 = HuffmanTree::createAndInsertNode(Root, 2, 0x01);
  BOOST_REQUIRE_EQUAL(Root->right()->right()->left(), Node_3_1);
}


BOOST_AUTO_TEST_CASE(createAndInsertNode_01) {
    const auto Root = createRoot();

    // Level 1
    const auto Node_0_1 = HuffmanTree::createAndInsertNode(Root, 1, 0x1);
    BOOST_REQUIRE_NE(Root->left(), nullptr);
    BOOST_REQUIRE_EQUAL(Root->left()->left(), Node_0_1);
    BOOST_REQUIRE_EQUAL(Root->left()->right(), nullptr);
    BOOST_REQUIRE_EQUAL(Root->right(), nullptr);

    // Level 2
    const auto Node_1_1 = HuffmanTree::createAndInsertNode(Root, 2, 0x0);
    BOOST_REQUIRE_NE(Root->left(), nullptr);
    BOOST_REQUIRE_NE(Root->left()->right(), nullptr);
    BOOST_REQUIRE_EQUAL(Root->left()->left(), Node_0_1);
    BOOST_REQUIRE_EQUAL(Root->left()->right()->left(), Node_1_1);
    BOOST_REQUIRE_EQUAL(Root->left()->right()->right(), nullptr);
    BOOST_REQUIRE_EQUAL(Root->right(), nullptr);

    const auto Node_1_2 = HuffmanTree::createAndInsertNode(Root, 2, 0x2);
    BOOST_REQUIRE_NE(Root->left(), nullptr);
    BOOST_REQUIRE_NE(Root->left()->right(), nullptr);
    BOOST_REQUIRE_EQUAL(Root->left()->left(), Node_0_1);
    BOOST_REQUIRE_EQUAL(Root->left()->right()->left(), Node_1_1);
    BOOST_REQUIRE_EQUAL(Root->left()->right()->right(), Node_1_2);
    BOOST_REQUIRE_EQUAL(Root->right(), nullptr);

    const auto Node_1_3 = HuffmanTree::createAndInsertNode(Root, 2, 0x3);
    BOOST_REQUIRE_NE(Root->left(), nullptr);
    BOOST_REQUIRE_NE(Root->left()->right(), nullptr);
    BOOST_REQUIRE_NE(Root->right(), nullptr);
    BOOST_REQUIRE_NE(Root->right()->left(), nullptr);
    BOOST_REQUIRE_EQUAL(Root->left()->left(), Node_0_1);
    BOOST_REQUIRE_EQUAL(Root->left()->right()->left(), Node_1_1);
    BOOST_REQUIRE_EQUAL(Root->left()->right()->right(), Node_1_2);
    BOOST_REQUIRE_EQUAL(Root->right()->left()->left(), Node_1_3);
    BOOST_REQUIRE_EQUAL(Root->right()->left()->right(), nullptr);
    BOOST_REQUIRE_EQUAL(Root->right()->right(), nullptr);

    const auto Node_1_4 = HuffmanTree::createAndInsertNode(Root, 2, 0x4);
    BOOST_REQUIRE_NE(Root->left(), nullptr);
    BOOST_REQUIRE_NE(Root->left()->right(), nullptr);
    BOOST_REQUIRE_NE(Root->right(), nullptr);
    BOOST_REQUIRE_NE(Root->right()->left(), nullptr);
    BOOST_REQUIRE_EQUAL(Root->left()->left(), Node_0_1);
    BOOST_REQUIRE_EQUAL(Root->left()->right()->left(), Node_1_1);
    BOOST_REQUIRE_EQUAL(Root->left()->right()->right(), Node_1_2);
    BOOST_REQUIRE_EQUAL(Root->right()->left()->left(), Node_1_3);
    BOOST_REQUIRE_EQUAL(Root->right()->left()->right(), Node_1_4);
    BOOST_REQUIRE_EQUAL(Root->right()->right(), nullptr);

    // Level 3
    const auto Node_2_1 = HuffmanTree::createAndInsertNode(Root, 3, 0x5);
    BOOST_REQUIRE_NE(Root->left(), nullptr);
    BOOST_REQUIRE_NE(Root->left()->right(), nullptr);
    BOOST_REQUIRE_NE(Root->right(), nullptr);
    BOOST_REQUIRE_NE(Root->right()->left(), nullptr);
    BOOST_REQUIRE_NE(Root->right()->right(), nullptr);
    BOOST_REQUIRE_NE(Root->right()->right()->left(), nullptr);
    BOOST_REQUIRE_EQUAL(Root->left()->left(), Node_0_1);
    BOOST_REQUIRE_EQUAL(Root->left()->right()->left(), Node_1_1);
    BOOST_REQUIRE_EQUAL(Root->left()->right()->right(), Node_1_2);
    BOOST_REQUIRE_EQUAL(Root->right()->left()->left(), Node_1_3);
    BOOST_REQUIRE_EQUAL(Root->right()->left()->right(), Node_1_4);
    BOOST_REQUIRE_EQUAL(Root->right()->right()->left()->right(), nullptr);
    BOOST_REQUIRE_EQUAL(Root->right()->right()->left()->left(), Node_2_1);
    BOOST_REQUIRE_EQUAL(Root->right()->right()->right(), nullptr);
}

BOOST_AUTO_TEST_SUITE_END()
