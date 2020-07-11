#include "huffman_tree.hpp"

#include <boost/assert.hpp>

auto HuffmanTree::createAndinsertNodeImplementation(
    std::shared_ptr<Node> const &Parent, uint8_t const Level,
    uint8_t const Value, uint8_t const CurrentLevel,
    BypassDirection const Direction) -> std::shared_ptr<HuffmanTree::Node> {
  using Node = HuffmanTree::Node;

  // when reach correct level - try to create leaf
  if (Level == CurrentLevel) {
    if (nullptr != Parent->Left && nullptr != Parent->Right) {
      return nullptr;
    }

    auto item = std::make_shared<Node>(Parent, Value);

    if (nullptr == Parent->Left) {
      Parent->Left = item;
    } else if (nullptr == Parent->Right) {
      Parent->Right = item;
    }
    return item;
  }

  uint8_t nextLevel = CurrentLevel;
  std::shared_ptr<Node> node;
  auto NewDirection = BypassDirection::Down;

  const auto checkChild = [&](auto &child) {
    if (nullptr == child) {
      child = std::make_shared<Node>(Parent);
      node = child;
      nextLevel = CurrentLevel + 1;
      return true;
    } else if (!child->isLeaf()) {
      node = child;
      nextLevel = CurrentLevel + 1;
      return true;
    }
    return false;
  };

  const auto checkUp = [&](auto parent) {
    if (nextLevel == 0) {
      throw std::runtime_error("Tree is filled");
    }

    node = parent->Parent.lock();
    nextLevel = CurrentLevel - 1;
    return false;
  };

  if (BypassDirection::Down == Direction) {
    bool checked = checkChild(Parent->Left);
    if (!checked) {
      checked = checkChild(Parent->Right);
    }
    if (!checked) {
      NewDirection = BypassDirection::Up;
      checkUp(Parent);
    } else {
      NewDirection = BypassDirection::Down;
    }
  } else {
    bool checked = checkChild(Parent->Right);
    if (!checked) {
      NewDirection = BypassDirection::Up;
      checkUp(Parent);
    } else {
      NewDirection = BypassDirection::Down;
    }
  }

  auto inserted = createAndinsertNodeImplementation(node, Level, Value,
                                                    nextLevel, NewDirection);

  if ((inserted != nullptr) || (CurrentLevel == 0)) {
    return inserted;
  }

  NewDirection = BypassDirection::Up;
  return createAndinsertNodeImplementation(node, Level, Value, nextLevel,
                                           NewDirection);
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

auto HuffmanTree::createAndinsertNode(std::shared_ptr<Node> const &Parent,
                                      uint8_t Level, uint8_t Value)
    -> std::shared_ptr<Node> {
  return createAndinsertNodeImplementation(Parent, Level, Value);
}

auto HuffmanTree::Node::Builder::left(const uint8_t Value) -> Builder & {
  NextNode->Left = std::make_shared<Node>(NextNode, Value);
  return *this;
}

auto HuffmanTree::Node::Builder::right(const uint8_t Value) -> Builder & {
  NextNode->Right = std::make_shared<Node>(NextNode, Value);
  return *this;
}

auto HuffmanTree::Node::Builder::left() -> Builder & {
  NextNode->Left = std::make_shared<Node>(NextNode);
  NextNode = NextNode->Left;
  return *this;
}

auto HuffmanTree::Node::Builder::right() -> Builder & {
  NextNode->Right = std::make_shared<Node>(NextNode);
  NextNode = NextNode->Right;
  return *this;
}

auto HuffmanTree::Node::Builder::end() -> Builder & {
  const auto ParentNode = NextNode->Parent.lock();
  assert(ParentNode != nullptr);
  NextNode = ParentNode;
  return *this;
}

auto HuffmanTree::Node::Builder::done() -> std::shared_ptr<HuffmanTree::Node> {
  assert(Root == NextNode);
  return Root;
}
