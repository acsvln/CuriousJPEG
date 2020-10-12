#ifndef SOS_DECODER_HPP
#define SOS_DECODER_HPP

#include "decoder.hpp"

#include <bitset>

#include "bit_extractor.hpp"

class SOSDecoder : public Decoder {
protected:
  using DataUnit = boost::numeric::ublas::matrix<int8_t>;

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
                          std::shared_ptr<HuffmanTree::Node> const &Tree)
      -> std::shared_ptr<HuffmanTree::Node>;

  static auto
  readDU(BitExtractor &,
                     const std::shared_ptr<HuffmanTree::Node> &DC_Table,
                     const std::shared_ptr<HuffmanTree::Node> &AC_Table)
      -> DataUnit;

  static auto
  readMCU(BitExtractor &m, DCTTable const &dct,
          std::vector<Channel> const &channels,
          std::vector<std::shared_ptr<HuffmanTree::Node>> const& AC_Tables,
          std::vector<std::shared_ptr<HuffmanTree::Node>> const& DC_Tables)
  -> MinimumCodedUnit;

  static auto
  quantMCU(MinimumCodedUnit&& mcu, std::vector<DCTComponent> const& components,
                     std::vector<boost::numeric::ublas::matrix<uint16_t>> const& Quant) -> MinimumCodedUnit;

  static auto
  reverseDQT(boost::numeric::ublas::matrix<int16_t> const& matrix) -> boost::numeric::ublas::matrix<int16_t>;

  static auto
  reverseDQT2(boost::numeric::ublas::matrix<int16_t> const& matrix) -> boost::numeric::ublas::matrix<int16_t>;

  static auto
  normalizeReversedDQT(boost::numeric::ublas::matrix<int16_t>&& Table) -> boost::numeric::ublas::matrix<int16_t>;

  static auto
  convertYCbCrToRGB(boost::numeric::ublas::matrix<int16_t> const &y,
             boost::numeric::ublas::matrix<int16_t> const &cb,
             boost::numeric::ublas::matrix<int16_t> const &cr) ->
    std::tuple<
        boost::numeric::ublas::matrix<int16_t>,
        boost::numeric::ublas::matrix<int16_t>,
        boost::numeric::ublas::matrix<int16_t>
  >;  

public:
  ~SOSDecoder() override = default;
  SOSDecoder();

  static auto
  convertYCbCrToRGB_AL(boost::numeric::ublas::matrix<int16_t> const &y,
             boost::numeric::ublas::matrix<int16_t> const &cb,
             boost::numeric::ublas::matrix<int16_t> const &cr) ->
    std::tuple<
        boost::numeric::ublas::matrix<int16_t>,
        boost::numeric::ublas::matrix<int16_t>,
        boost::numeric::ublas::matrix<int16_t>
  >;

#if 0

  static auto
  convertYCbCrToRGB_2(boost::numeric::ublas::matrix<int16_t> const &y,
             boost::numeric::ublas::matrix<int16_t> const &cb,
             boost::numeric::ublas::matrix<int16_t> const &cr) ->
    std::tuple<
        boost::numeric::ublas::matrix<int16_t>,
        boost::numeric::ublas::matrix<int16_t>,
        boost::numeric::ublas::matrix<int16_t>
  >;

  static
  auto
  convertYCbCrToRGB_3(
        boost::numeric::ublas::matrix<int16_t> const& Y
      , boost::numeric::ublas::matrix<int16_t> const& Cb
      , boost::numeric::ublas::matrix<int16_t> const& Cr ) -> std::tuple<
        boost::numeric::ublas::matrix<int16_t>
      , boost::numeric::ublas::matrix<int16_t>
      , boost::numeric::ublas::matrix<int16_t>
  >;

#endif

private:
  void InvokeImpl(std::istream &Stream, Context &Ctx) final;

};

#endif // SOS_DECODER_HPP
