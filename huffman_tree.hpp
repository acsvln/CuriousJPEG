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
    Node(const std::shared_ptr<Node> &BaseNode);
    Node(const std::shared_ptr<Node> &BaseNode, const uint8_t Value);

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

  static auto createAndInsertNode(const std::shared_ptr<Node> &Parent,
                                  const std::size_t Level, const uint8_t Value)
      -> std::shared_ptr<Node>;

public:
  class Builder final {
  public:
    auto left() -> Builder &;
    auto right() -> Builder &;

    auto left(const uint8_t Value) -> Builder &;
    auto right(const uint8_t Value) -> Builder &;

    auto end() -> Builder &;
    auto done() -> std::shared_ptr<Node>;

  private:
    std::shared_ptr<Node> Root = std::make_shared<Node>(nullptr);
    std::shared_ptr<Node> NextNode = Root;
  };

private:
  enum class BypassDirection { Up, Down };
  using LevelValue = std::tuple<std::size_t, uint8_t>;

  static auto createAndInsertNodeImplementation(
      const std::shared_ptr<Node> &Parent, const LevelValue& Data,
          const std::size_t CurrentLevel = 0,
      const BypassDirection Direction = BypassDirection::Down)
      -> std::shared_ptr<Node>;
};

#endif // HUFFMANTREE_HPP
