#include "sof0_decoder.hpp"

#include <iostream>

#include "utility.hpp"

void SOF0Decoder::Invoke(std::istream& aStream, Context& aContext)
{
    std::cout << "Invoking SOF0Decoder" << std::endl;

    uint16_t size;
    aStream.read(reinterpret_cast<char*>(&size),sizeof(uint16_t));
    size = swap_endian(size);

    uint8_t precision;
    uint16_t height;
    uint16_t width;

    aStream.read(reinterpret_cast<char*>(&precision),sizeof(uint8_t));
    aStream.read(reinterpret_cast<char*>(&height),sizeof(uint16_t));
    aStream.read(reinterpret_cast<char*>(&width),sizeof(uint16_t));

    height = swap_endian(height);
    width = swap_endian(width);

    aContext.dct.precision = precision;
    aContext.dct.height = height;
    aContext.dct.width = width;

    uint8_t unitsCount = 0x00;

    aStream.read(reinterpret_cast<char*>(&unitsCount),sizeof(unitsCount));
    aContext.dct.components.resize(unitsCount);

    uint32_t maxH = 0, maxV = 0;

    for (int cx=0; cx<unitsCount; cx++)
    {
        DCTComponent component;

        aStream.read((char*)&component,sizeof(DCTComponent)); // TODO: Remove that shit

        if (component.h > maxH) {
            aContext.dct.maxH = component.h;
        }

        if (component.v > maxV) {
            aContext.dct.maxV = component.v;
        }

        aContext.dct.components[cx] = component;
    }
}
