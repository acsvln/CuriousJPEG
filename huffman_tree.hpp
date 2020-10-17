#ifndef HUFFMANTREE_HPP
#define HUFFMANTREE_HPP

#include <cstdint>
#include <memory>
#include <optional>

class HuffmanTree final {
public:
  class Node final {
    friend class HuffmanTree;
    friend class Builder;

  public:
    Node(std::shared_ptr<Node> const &BaseNode);
    Node(std::shared_ptr<Node> const &BaseNode, uint8_t const Value);

    auto isLeaf() const noexcept -> bool;
    auto left() const -> std::shared_ptr<Node>;
    auto right() const -> std::shared_ptr<Node>;
    auto parent() const -> std::weak_ptr<Node>;
    auto data() const -> uint8_t;

  private:
    std::shared_ptr<Node> Left = nullptr;
    std::shared_ptr<Node> Right = nullptr;
    std::weak_ptr<Node> Parent;
    std::optional<uint8_t> Data = {};
  };

  static auto createAndInsertNode(std::shared_ptr<Node> const &Parent,
                                  std::size_t const Level, uint8_t const Value)
      -> std::shared_ptr<Node>;

public:
  class Builder final {
  public:
    auto left() -> Builder &;
    auto right() -> Builder &;

    auto left(uint8_t const Value) -> Builder &;
    auto right(uint8_t const Value) -> Builder &;

    auto end() -> Builder &;
    auto done() -> std::shared_ptr<Node>;

  private:
    std::shared_ptr<Node> Root = std::make_shared<Node>(nullptr);
    std::shared_ptr<Node> NextNode = Root;
  };

private:
  enum class BypassDirection { Up, Down };

  static auto createAndInsertNodeImplementation(
      std::shared_ptr<Node> const &Parent, std::size_t const Level,
      uint8_t const Value, std::size_t const CurrentLevel = 0,
      BypassDirection const Direction = BypassDirection::Down)
      -> std::shared_ptr<Node>;
};

#endif // HUFFMANTREE_HPP
