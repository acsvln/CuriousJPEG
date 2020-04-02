#ifndef SOS_DECODER_HPP
#define SOS_DECODER_HPP

#include "decoder.hpp"

#include <bitset>

class SOSDecoder : public IDecoder
{
public:
    class BitExtractor final {
    public:
        explicit
        BitExtractor( std::istream &aStream );
        uint8_t nextNumber( std::size_t bit_cnt = 1 );

    private:

        std::bitset<8> mbits;
        std::size_t mCounter = 0;
        std::istream &mStream;
    };

    static
    std::shared_ptr<DHTNode> LocateNodeInTree(
        BitExtractor& extractor,
        const std::shared_ptr<DHTNode>& AC_Table );

    static
    boost::numeric::ublas::matrix<uint8_t> ReadMatrix(
        BitExtractor&,
        const std::shared_ptr<DHTNode>& AC_Table,
        const std::shared_ptr<DHTNode>& DC_Table );

public:
    ~SOSDecoder() final = default;

    // IDecoder interface
public:
    void Invoke(std::istream &aStream, Context& aContext) final;
};

#endif // SOS_DECODER_HPP
