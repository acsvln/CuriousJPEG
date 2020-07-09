#include "decoder.hpp"

#include <cassert>

#include "data_reader.hpp"
#include "utility.hpp"

Decoder::Decoder(const std::string &Header) : DecoderHeader{Header} {}

void Decoder::Invoke(std::istream &Stream, Context &Ctx) {
  const auto Size = DataReader::readNumber<uint16_t>(Stream);
  std::cout << "Segment started: " << DecoderHeader << std::endl;
  std::cout << "Segment size: " << Size << " bytes" << std::endl;
  const auto Pos = Stream.tellg();
  InvokeImpl(Stream, Ctx);
  std::cout << "x" << (Stream.tellg() - Pos) << "y" <<(Size - sizeof(Size));

  assert((Stream.tellg() - Pos) == (Size - sizeof(Size)));
}
