#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <memory>
#include <optional>
#include <vector>

struct DHTNode
{
    std::shared_ptr<DHTNode> left = nullptr;
    std::shared_ptr<DHTNode> right = nullptr;
    std::weak_ptr<DHTNode> parent;
    std::optional<uint8_t> data = {};

    bool IsLeaf() const {
        return data.has_value();
    }
};

struct Context
{
    std::vector<DHTNode> AC_HuffmanTables;
    std::vector<DHTNode> DC_HuffmanTables;
};

#endif // CONTEXT_HPP
