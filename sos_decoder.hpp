#ifndef SOS_DECODER_HPP
#define SOS_DECODER_HPP

#include "idecoder.hpp"

#include <bitset>

#include "bit_extractor.hpp"

class SOSDecoder : public IDecoder
{
public:

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
    std::shared_ptr<HuffmanTree::Node> LocateNodeInTree(
        BitExtractor& extractor,
        const std::shared_ptr<HuffmanTree::Node>& AC_Table );

    static
    boost::numeric::ublas::matrix<uint8_t> ReadMatrix(
        BitExtractor&,
        const std::shared_ptr<HuffmanTree::Node>& DC_Table,
        const std::shared_ptr<HuffmanTree::Node>& AC_Table );

    static
    Cs ReadMCU(
        BitExtractor&m,
        DCTTable const& dct,
        std::vector<Channel>const& channels,
        std::vector<std::shared_ptr<HuffmanTree::Node>> AC_HuffmanTables,
        std::vector<std::shared_ptr<HuffmanTree::Node>> DC_HuffmanTables );

    static
    Cs QuantMCU(
        Cs mcu
        , std::vector<DCTComponent> const& components
        , std::vector<boost::numeric::ublas::matrix<uint8_t>> quant );

    static
    boost::numeric::ublas::matrix<int16_t> ReverseDQT_1(
        boost::numeric::ublas::matrix<int16_t> const& matrix );

    static
    boost::numeric::ublas::matrix<int8_t> ReverseDQT3(
        boost::numeric::ublas::matrix<int8_t> const& matrix );

    static
    std::tuple<
          boost::numeric::ublas::matrix<int16_t>
        , boost::numeric::ublas::matrix<int16_t>
        , boost::numeric::ublas::matrix<int16_t>>
    YCbCrToRGB(
          boost::numeric::ublas::matrix<int16_t> const& y
        , boost::numeric::ublas::matrix<int16_t> const& cb
        , boost::numeric::ublas::matrix<int16_t> const& cr );

public:
    ~SOSDecoder() final = default;

    // IDecoder interface
public:
    void Invoke(std::istream &aStream, Context& aContext) final;
};

#endif // SOS_DECODER_HPP
