#include "sof0_decoder.hpp"

#include <iostream>

#include "utility.hpp"

void SOF0Decoder::Invoke(std::istream& aStream, Context& aContext)
{
    const auto size = ReadNumberFromStream<uint16_t>(aStream);
    printSectionDescription("Baseline DCT", size);

    const auto precision = ReadNumberFromStream<uint8_t>(aStream);
    const auto height = ReadNumberFromStream<uint16_t>(aStream);
    const auto width = ReadNumberFromStream<uint16_t>(aStream);

    aContext.dct.precision = precision;
    aContext.dct.height = height;
    aContext.dct.width = width;

    const auto unitsCount = ReadNumberFromStream<uint8_t>(aStream);
    aContext.dct.components.resize(unitsCount);

    uint32_t maxH = 0, maxV = 0;

    for (int cx=0; cx<unitsCount; cx++)
    {
        DCTComponent component;

        // TODO: Remove that shit
        aStream.read((char*)&component,sizeof(DCTComponent));

        if (component.h > maxH) {
            aContext.dct.maxH = component.h;
        }

        if (component.v > maxV) {
            aContext.dct.maxV = component.v;
        }

        aContext.dct.components[cx] = component;
    }
}
