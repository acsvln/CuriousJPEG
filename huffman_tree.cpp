#include "huffman_tree.hpp"

#include <boost/assert.hpp>
#include <stdexcept>
#include <iostream>

auto HuffmanTree::createAndInsertNodeImplementation(
    const std::shared_ptr<Node> &Parent, const LevelValue& Data, const std::size_t CurrentLevel,
    const BypassDirection Direction) -> std::shared_ptr<Node> {
  // when reach correct level - try to create leaf
  if (const auto [Level, Value] = Data; Level == CurrentLevel) {
    if (nullptr != Parent->Left && nullptr != Parent->Right) {
      return nullptr;
    }
    const auto Item = std::make_shared<Node>(Parent, Value);
    if (nullptr == Parent->Left) {
      Parent->Left = Item;
    } else if (nullptr == Parent->Right) {
      Parent->Right = Item;
    }
    return Item;
  }

  const auto createOrResolveChild = [&](auto &Child) -> std::shared_ptr<Node> {
    if (nullptr == Child) {
      Child = std::make_shared<Node>(Parent);
    } else if (Child->isLeaf()) {
      return nullptr;
    }
    return Child;
  };

  // Walk left
  if (BypassDirection::Up != Direction) {
    if (const auto LeftChild = createOrResolveChild(Parent->Left);
        nullptr != LeftChild) {
      const auto Inserted = createAndInsertNodeImplementation(
          LeftChild, Data, CurrentLevel + 1, BypassDirection::Down);
      if (nullptr != Inserted) {
        return Inserted;
      }
    }
  }

  // Walk right
  if (const auto RightChild = createOrResolveChild(Parent->Right);
      nullptr != RightChild) {
    const auto Inserted = createAndInsertNodeImplementation(
        RightChild, Data, CurrentLevel + 1, BypassDirection::Down);
    if (nullptr != Inserted) {
      return Inserted;
    }
  }

  return nullptr;
}

HuffmanTree::Node::Node(const std::shared_ptr<HuffmanTree::Node> &BaseNode)
    : Parent{BaseNode} {}

HuffmanTree::Node::Node(const std::shared_ptr<HuffmanTree::Node> &BaseNode,
                        uint8_t Value)
    : Parent{BaseNode}, Data{Value} {}

auto HuffmanTree::Node::isLeaf() const noexcept -> bool {
  return Data.has_value();
}

auto HuffmanTree::Node::left() const -> std::shared_ptr<Node> { return Left; }
auto HuffmanTree::Node::right() const -> std::shared_ptr<Node> { return Right; }
auto HuffmanTree::Node::parent() const -> std::weak_ptr<Node> { return Parent; }
auto HuffmanTree::Node::data() const -> uint8_t {
  BOOST_ASSERT_MSG(Data.has_value(), "Node is not leaf");
  return Data.value();
}

auto HuffmanTree::createAndInsertNode(const std::shared_ptr<Node> &Parent,
                                      std::size_t Level, uint8_t Value)
    -> std::shared_ptr<Node> {
  return createAndInsertNodeImplementation(Parent, {Level, Value});
}

auto HuffmanTree::Builder::left(const uint8_t Value) -> Builder & {
  NextNode->Left = std::make_shared<Node>(NextNode, Value);
  return *this;
}

auto HuffmanTree::Builder::right(const uint8_t Value) -> Builder & {
  NextNode->Right = std::make_shared<Node>(NextNode, Value);
  return *this;
}

auto HuffmanTree::Builder::left() -> Builder & {
  NextNode->Left = std::make_shared<Node>(NextNode);
  NextNode = NextNode->Left;
  return *this;
}

auto HuffmanTree::Builder::right() -> Builder & {
  NextNode->Right = std::make_shared<Node>(NextNode);
  NextNode = NextNode->Right;
  return *this;
}

auto HuffmanTree::Builder::end() -> Builder & {
  const auto ParentNode = NextNode->Parent.lock();
  BOOST_ASSERT_MSG(ParentNode != nullptr,
                   "Don't call end method on the root of tree");
  NextNode = ParentNode;
  return *this;
}

auto HuffmanTree::Builder::done() -> std::shared_ptr<HuffmanTree::Node> {
  assert(Root == NextNode);
  BOOST_ASSERT_MSG(Root == NextNode, "Tree was not fully formed");
  return Root;
}
