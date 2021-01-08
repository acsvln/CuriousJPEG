#include "sof0_decoder.hpp"

#include "data_reader.hpp"

SOF0Decoder::SOF0Decoder() : DecoderBase{"Baseline DCT"} {}

void SOF0Decoder::InvokeImpl(std::istream &Stream, DecoderContext &Context) {
  const auto Precision = DataReader::readNumber<uint8_t>(Stream);
  const auto Height = DataReader::readNumber<uint16_t>(Stream);
  const auto Width = DataReader::readNumber<uint16_t>(Stream);

  Context.dct.Precision = Precision;
  Context.dct.Height = Height;
  Context.dct.Width = Width;

  const auto UnitsCount = DataReader::readNumber<uint8_t>(Stream);
  Context.dct.Components.resize(UnitsCount);

  uint32_t MaxH = 0, MaxV = 0;

  for (std::size_t Index = 0; Index < UnitsCount; Index++) {
    auto &Component = Context.dct.Components[Index];
    DataReader::readSruct(Stream, Component);

    if (Component.H > MaxH) {
      MaxH = Component.H;
    }

    if (Component.V > MaxV) {
      MaxV = Component.V;
    }
  }

  Context.dct.MaxH = MaxH;
  Context.dct.MaxV = MaxV;
}
