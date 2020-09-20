#include "sof0_decoder.hpp"

#include <iostream>

#include "data_reader.hpp"
#include "utility.hpp"

SOF0Decoder::SOF0Decoder() : Decoder{"Baseline DCT"} {}

void SOF0Decoder::InvokeImpl(std::istream &Stream, Context &Ctx) {
  const auto Precision = DataReader::readNumber<uint8_t>(Stream);
  const auto Height = DataReader::readNumber<uint16_t>(Stream);
  const auto Width = DataReader::readNumber<uint16_t>(Stream);

  Ctx.dct.Precision = Precision;
  Ctx.dct.Height = Height;
  Ctx.dct.Width = Width;

  const auto UnitsCount = DataReader::readNumber<uint8_t>(Stream);
  Ctx.dct.Components.resize(UnitsCount);

  uint32_t MaxH = 0, MaxV = 0;

  for (std::size_t Index = 0; Index < UnitsCount; Index++) {
    auto &component = Ctx.dct.Components[Index];
    DataReader::readSruct(Stream, component);

    if (component.H > MaxH) {
      MaxH = component.H;
    }

    if (component.V > MaxV) {
      MaxV = component.V;
    }
  }

  Ctx.dct.MaxH = MaxH;
  Ctx.dct.MaxV = MaxV;
}
