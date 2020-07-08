#include "sof0_decoder.hpp"

#include <iostream>

#include "utility.hpp"
#include "data_reader.hpp"

void SOF0Decoder::Invoke(std::istream& aStream, Context& aContext)
{
    const auto size = DataReader::readNumber<uint16_t>(aStream);
    printSectionDescription("Baseline DCT", size);

    const auto precision = DataReader::readNumber<uint8_t>(aStream);
    const auto height = DataReader::readNumber<uint16_t>(aStream);
    const auto width = DataReader::readNumber<uint16_t>(aStream);

    aContext.dct.precision = precision;
    aContext.dct.height = height;
    aContext.dct.width = width;

    const auto unitsCount = DataReader::readNumber<uint8_t>(aStream);
    aContext.dct.components.resize(unitsCount);

    uint32_t maxH = 0, maxV = 0;

    for (int cx=0; cx<unitsCount; cx++)
    {
        DCTComponent component;
        DataReader::readSruct(aStream, component);

        // TODO: For max/min write test

        if (component.h > maxH) {
            maxH = component.h;
        }

        if (component.v > maxV) {
            maxV = component.v;
        }        

        aContext.dct.components[cx] = component;
    }

    aContext.dct.maxH = maxH;
    aContext.dct.maxV = maxV;
}
