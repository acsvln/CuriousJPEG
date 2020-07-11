#include "decoder.hpp"

#include <boost/assert.hpp>

#include "data_reader.hpp"
#include "utility.hpp"

Decoder::Decoder(const std::string &Header) : DecoderHeader{Header} {}

void Decoder::Invoke(std::istream &Stream, Context &Ctx) {
  const auto Size = DataReader::readNumber<uint16_t>(Stream);
  std::cout << "Segment started: " << DecoderHeader << std::endl;
  std::cout << "Segment size: " << Size << " bytes" << std::endl;
  const auto Pos = Stream.tellg();
  InvokeImpl(Stream, Ctx);
  BOOST_ASSERT_MSG((Stream.tellg() - Pos) == (Size - sizeof(Size)),
                   "Segment was not read correctly");
}
