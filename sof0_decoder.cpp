#include "sof0_decoder.hpp"

#include <iostream>

#include "data_reader.hpp"
#include "utility.hpp"

SOF0Decoder::SOF0Decoder() : Decoder{"Baseline DCT"} {}

void SOF0Decoder::InvokeImpl(std::istream &Stream, Context &Ctx) {
  const auto precision = DataReader::readNumber<uint8_t>(Stream);
  const auto height = DataReader::readNumber<uint16_t>(Stream);
  const auto width = DataReader::readNumber<uint16_t>(Stream);

  Ctx.dct.precision = precision;
  Ctx.dct.height = height;
  Ctx.dct.width = width;

  const auto unitsCount = DataReader::readNumber<uint8_t>(Stream);
  Ctx.dct.components.resize(unitsCount);

  uint32_t maxH = 0, maxV = 0;

  for (std::size_t cx = 0; cx < unitsCount; cx++) {
    auto &component = Ctx.dct.components[cx];
    DataReader::readSruct(Stream, component);

    // TODO: Test that
    if (component.h > maxH) {
      maxH = component.h;
    }

    if (component.v > maxV) {
      maxV = component.v;
    }
  }

  Ctx.dct.maxH = maxH;
  Ctx.dct.maxV = maxV;
}
