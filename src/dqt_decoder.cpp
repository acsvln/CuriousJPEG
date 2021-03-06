#include "dqt_decoder.hpp"

#include "data_reader.hpp"
#include "exceptions.hpp"
#include "utility.hpp"

DQTDecoder::DQTDecoder() : DecoderBase{"Define a Quantization Table"} {}

void DQTDecoder::InvokeImpl(std::istream &Stream, DecoderContext &Context) {
  const auto TableDescription = DataReader::readNumber<uint8_t>(Stream);

  const auto TableElementSize = highByte(TableDescription);
  if (TableElementSize != 0) {
      throw NotImplementedException{ "Element sizes of DQT table elementes can not be " + std::to_string(TableElementSize) };
  }

  const auto TableIndex = lowByte(TableDescription);
  if (Context.DQT_Vector.size() <= TableIndex) {
    Context.DQT_Vector.resize(TableIndex + 1);
  }

  std::array<uint8_t, 64> RawBuffer;
  DataReader::readBuffer(Stream, RawBuffer);

  std::array<uint16_t, 64> Buffer;
  std::transform(std::begin(RawBuffer), std::end(RawBuffer), std::begin(Buffer),
                 TransformCaster<uint16_t>());

  Context.DQT_Vector[TableIndex] = CreateZigZagMatrix(Buffer);
}
