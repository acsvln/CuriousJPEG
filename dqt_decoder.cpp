#include "dqt_decoder.hpp"

#include "data_reader.hpp"
#include "exceptions.h"
#include "utility.hpp"

DQTDecoder::DQTDecoder() : Decoder{"Define a Quantization Table"} {}

void DQTDecoder::InvokeImpl(std::istream &Stream, Context &Ctx) {
  const auto numBuffer = DataReader::readNumber<uint8_t>(Stream);

  const auto tableElementSize = lowByte(numBuffer);
  if (tableElementSize != 0) {
      throw NotImplementedException{};
  }

  const auto tableIndex = highByte(numBuffer);
  if (Ctx.DQT_Vector.size() <= tableIndex) {
    Ctx.DQT_Vector.resize(tableIndex + 1);
  }

  std::array<uint8_t, 64> RawBuffer;
  DataReader::readBuffer(Stream, RawBuffer);

  std::array<uint16_t, 64> Buffer;
  std::transform(std::begin(RawBuffer), std::end(RawBuffer), std::begin(Buffer),
                 TransformCaster<uint16_t>());

  Ctx.DQT_Vector[tableIndex] = CreateZigZagMatrix(Buffer);
}
