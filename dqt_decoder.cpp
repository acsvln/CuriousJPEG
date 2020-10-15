#include "dqt_decoder.hpp"

#include "data_reader.hpp"
#include "exceptions.hpp"
#include "utility.hpp"

DQTDecoder::DQTDecoder() : Decoder{"Define a Quantization Table"} {}

void DQTDecoder::InvokeImpl(std::istream &Stream, Context &Ctx) {
  const auto NumBuffer = DataReader::readNumber<uint8_t>(Stream);

  const auto TableElementSize = lowByte(NumBuffer);
  if (TableElementSize != 0) {
    throw NotImplementedException{};
  }

  const auto TableIndex = highByte(NumBuffer);
  if (Ctx.DQT_Vector.size() <= TableIndex) {
    Ctx.DQT_Vector.resize(TableIndex + 1);
  }

  std::array<uint8_t, 64> RawBuffer;
  DataReader::readBuffer(Stream, RawBuffer);

  std::array<uint16_t, 64> Buffer;
  std::transform(std::begin(RawBuffer), std::end(RawBuffer), std::begin(Buffer),
                 TransformCaster<uint16_t>());

  Ctx.DQT_Vector[TableIndex] = CreateZigZagMatrix(Buffer);
}
