#include "decoder_base.hpp"

#include <boost/assert.hpp>

#include "data_reader.hpp"

DecoderBase::DecoderBase(const std::string &Header) : DecoderHeader{Header} {}

void DecoderBase::Invoke(std::istream &Stream, DecoderContext &Context) {
  const auto Size = DataReader::readNumber<uint16_t>(Stream);
  echoSegmentCaption(DecoderHeader, Size);
  const auto Pos = Stream.tellg();
  InvokeImpl(Stream, Context);
  BOOST_ASSERT_MSG((Stream.tellg() - Pos) == (Size - sizeof(Size)),
                   "Segment was not read correctly");
}

void DecoderBase::echoSegmentCaption(const std::string &Header,
                                     const uint16_t Size) {
  std::cout << "Segment started: " << Header << std::endl;
  std::cout << "Segment size: " << Size << " bytes" << std::endl;
}
