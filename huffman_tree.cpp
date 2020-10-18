#include "huffman_tree.hpp"

#include <boost/assert.hpp>
#include <stdexcept>

auto HuffmanTree::createAndInsertNodeImplementation(
    const std::shared_ptr<Node> &Parent, const std::size_t Level,
    const uint8_t Value, const std::size_t CurrentLevel,
    const BypassDirection Direction) -> std::shared_ptr<Node> {
  // when reach correct level - try to create leaf
  if (Level == CurrentLevel) {
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

  std::size_t NextLevel = CurrentLevel;
  std::shared_ptr<Node> NodeToInsertTo = nullptr;

  const auto checkChild = [&](auto &Child) {
    if (nullptr == Child) {
      Child = std::make_shared<Node>(Parent);
      NodeToInsertTo = Child;
      NextLevel = CurrentLevel + 1;
      return true;
    } else if (!Child->isLeaf()) {
      NodeToInsertTo = Child;
      NextLevel = CurrentLevel + 1;
      return true;
    }
    return false;
  };

  const auto checkUp = [&](auto Parent) {
    if (NextLevel == 0) {
      throw std::runtime_error("Tree is filled");
    }

    NodeToInsertTo = Parent->Parent.lock();
    NextLevel = CurrentLevel - 1;
    return false;
  };

  auto NewDirection = BypassDirection::Down;

  if (BypassDirection::Down == Direction) {
    bool IsChecked = checkChild(Parent->Left);
    if (!IsChecked) {
      IsChecked = checkChild(Parent->Right);
    }
    if (!IsChecked) {
      NewDirection = BypassDirection::Up;
      checkUp(Parent);
    } else {
      NewDirection = BypassDirection::Down;
    }
  } else {
    const bool IsChecked = checkChild(Parent->Right);
    if (!IsChecked) {
      NewDirection = BypassDirection::Up;
      checkUp(Parent);
    } else {
      NewDirection = BypassDirection::Down;
    }
  }

  const auto Inserted = createAndInsertNodeImplementation(
      NodeToInsertTo, Level, Value, NextLevel, NewDirection);

  if ((Inserted != nullptr) || (CurrentLevel == 0)) {
    return Inserted;
  }

  NewDirection = BypassDirection::Up;
  return createAndInsertNodeImplementation(NodeToInsertTo, Level, Value,
                                           NextLevel, NewDirection);
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
  return createAndInsertNodeImplementation(Parent, Level, Value);
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
