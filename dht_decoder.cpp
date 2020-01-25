#include "dht_decoder.hpp"

#include <iostream>

namespace {

std::shared_ptr<DHTNode> PushHuffValueImplementation(std::shared_ptr<DHTNode> parent, uint8_t level, uint8_t value, uint8_t currentLevel  = 0, bool direction = false)
{
    // when reach correct level - try to create leaf
    if (level == currentLevel) {
        if (nullptr!=parent->left && nullptr!=parent->right) {
            return nullptr;
        }

        auto item = std::make_shared<DHTNode>();
        item->data = value;
        item->parent=parent;

        if (nullptr==parent->left)
        {
            parent->left = item;
        }
        else if(nullptr==parent->right)
        {
            parent->right = item;
        }
        return item;
    }

    uint8_t nextLevel = currentLevel;
    std::shared_ptr<DHTNode> node;
    bool toUp = false;

    const auto checkChild = [&](auto& child){
        if (nullptr == child){
            child = std::make_shared<DHTNode>();
            child->parent = parent;
            node = child;
            nextLevel = currentLevel + 1;
            return true;
        } else if (!child->IsLeaf()) {
            node = child;
            nextLevel = currentLevel + 1;
            return true;
        }
        return false;
    };

    const auto checkUp = [&](auto parent){
        if (nextLevel == 0)
        {
            throw std::runtime_error("Tree is filled");
        }

        node = parent->parent.lock();
        nextLevel = currentLevel - 1;
        return false;
    };

    if ( !direction ) {
        bool checked = checkChild(parent->left);
        if (!checked) {
            checked =  checkChild(parent->right);
        }
        if (!checked) {
            toUp = true;
            checkUp(parent);
        } else {
            toUp = false;
        }
    } else {
        bool checked = checkChild(parent->right);
        if (!checked) {
            toUp = true;
            checkUp(parent);
        } else {
            toUp = false;
        }
    }

    auto inserted = PushHuffValueImplementation(node, level, value, nextLevel, toUp);

    if ( (inserted != nullptr) || ( currentLevel == 0) ) {
        return inserted;
    }

    toUp = true;
    return PushHuffValueImplementation(node, level, value, nextLevel, toUp);
}

}





void DHTDecoder::Invoke(std::istream &aStream, Context &aContext)
{
    static constexpr std::size_t HuffCodeCount = 16;
    static constexpr uint8_t HuffClassDC = 0u;
    static constexpr uint8_t HuffClassAC = 1u;

    const auto extractHuffTableVector = [&](const uint8_t tableClass)
            -> std::vector<DHTNode>& {
        switch (tableClass) {
        case HuffClassDC:
            return aContext.AC_HuffmanTables;
        case HuffClassAC:
            return aContext.DC_HuffmanTables;
        }

        std::stringstream ss;
        ss << "Invalid table class " << tableClass;
        throw std::runtime_error(ss.str());
    };

    const auto size = ReadNumberFromStream<uint16_t>(aStream);
    printSectionDescription("Define Huffman Table", size);

    const auto description = ReadNumberFromStream<uint8_t>(aStream);
    const uint8_t tableClass = description >> 4;
    const uint8_t tableIndex = description & 0xF;

    auto huffVector = extractHuffTableVector(tableClass);

    if (huffVector.size() < tableIndex ) {
        huffVector.resize(tableIndex);
    }

    const auto root = std::make_shared<DHTNode>();

    std::array<uint8_t, HuffCodeCount> huffCodes;

    static_assert( sizeof(std::istream::char_type) == sizeof(std::array<uint8_t, HuffCodeCount>::value_type) );
    static_assert( sizeof(std::istream::char_type*) == sizeof(std::array<uint8_t, HuffCodeCount>::pointer) );
    aStream.read(reinterpret_cast<std::istream::char_type*>(huffCodes.data()),HuffCodeCount);

    for (std::size_t index = 0; index<huffCodes.size(); ++index) {
        uint8_t count = huffCodes[index];
        if (count <= 0){
            continue;
        }

        std::vector<uint8_t> huffData{ count };
        aStream.read(reinterpret_cast<std::istream::char_type*>(huffData.data()),count);

        for (uint8_t code : huffData) {
            PushHuffValueImplementation(root, index, code, 0);
        }
    }
   //  huffVector[tableIndex] = root;
}

std::shared_ptr<DHTNode> PushHuffValue(std::shared_ptr<DHTNode> parent, uint8_t level, uint8_t value)
{
    return PushHuffValueImplementation(parent, level, value);
}
