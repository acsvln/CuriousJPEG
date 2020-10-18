#ifndef SOS_DECODER_HPP
#define SOS_DECODER_HPP

#include "decoder_base.hpp"

#include <bitset>

#include "bit_extractor.hpp"

class SOSDecoder : public DecoderBase {
protected:
  using Matrix8u = boost::numeric::ublas::matrix<uint8_t>;
  using Matrix16u = boost::numeric::ublas::matrix<uint16_t>;
  using Matrix16 = boost::numeric::ublas::matrix<int16_t>;

  using DataUnit = boost::numeric::ublas::matrix<int16_t>;

  struct MinimumCodedUnit {
    std::vector<DataUnit> Cs1;
    std::vector<DataUnit> Cs2;
    std::vector<DataUnit> Cs3;
    std::vector<DataUnit> Cs4;
  };

  struct Channel {
    std::size_t Id;
    std::size_t DC_Id;
    std::size_t AC_Id;
  };

  static auto
  locateNodeInHuffmanTree(BitExtractor &Extractor,
                          const std::shared_ptr<HuffmanTree::Node> &Tree)
      -> std::shared_ptr<HuffmanTree::Node>;

  static auto readDU(BitExtractor &Extractor,
                     const std::shared_ptr<HuffmanTree::Node> &DC_Table,
                     const std::shared_ptr<HuffmanTree::Node> &AC_Table)
      -> DataUnit;

  static auto
  readMCU(BitExtractor &Extractor, const DCTTable &DCT,
          const std::vector<Channel> &Channels,
          const std::vector<std::shared_ptr<HuffmanTree::Node>> &AC_Tables,
          const std::vector<std::shared_ptr<HuffmanTree::Node>> &DC_Tables)
      -> MinimumCodedUnit;

  static auto quantMCU(MinimumCodedUnit &&MCU,
                       const std::vector<DCTComponent> &Components,
                       const std::vector<Matrix16u> &DQTVector)
      -> MinimumCodedUnit;

  static auto reverseDQT(const Matrix16 &Matrix) -> Matrix16;

  static auto normalizeReversedDQT(Matrix16 &&Table) -> Matrix16;

  static auto convertYCbCrToRGB(const Matrix16 &Y, const Matrix16 &Cb,
                                const Matrix16 &Cr)
      -> std::tuple<Matrix8u, Matrix8u, Matrix8u>;

public:
  ~SOSDecoder() override = default;
  SOSDecoder();

private:
  void InvokeImpl(std::istream &Stream, DecoderContext &Context) final;
};

#endif // SOS_DECODER_HPP
