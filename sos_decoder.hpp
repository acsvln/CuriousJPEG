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

    struct Channel {
        std::size_t id;
        std::size_t dc_id;
        std::size_t ac_id;
    };

    struct Cs
    {
        std::vector<boost::numeric::ublas::matrix<uint8_t>> Cs1;
        std::vector<boost::numeric::ublas::matrix<uint8_t>> Cs2;
        std::vector<boost::numeric::ublas::matrix<uint8_t>> Cs3;
        std::vector<boost::numeric::ublas::matrix<uint8_t>> Cs4;
    };

    static
    std::shared_ptr<DHTNode> LocateNodeInTree(
        BitExtractor& extractor,
        const std::shared_ptr<DHTNode>& AC_Table );

    static
    boost::numeric::ublas::matrix<uint8_t> ReadMatrix(
        BitExtractor&,
        const std::shared_ptr<DHTNode>& DC_Table,
        const std::shared_ptr<DHTNode>& AC_Table );

    static
    Cs ReadMCU(
        BitExtractor&m,
        DCTTable const& dct,
        std::vector<Channel>const& channels,
        std::vector<std::shared_ptr<DHTNode>> AC_HuffmanTables,
        std::vector<std::shared_ptr<DHTNode>> DC_HuffmanTables );

    static
    Cs QuantMCU(
        Cs mcu
        , std::vector<DCTComponent> const& components
        , std::vector<boost::numeric::ublas::matrix<uint8_t>> quant );

public:
    ~SOSDecoder() final = default;

    // IDecoder interface
public:
    void Invoke(std::istream &aStream, Context& aContext) final;
};

#endif // SOS_DECODER_HPP
