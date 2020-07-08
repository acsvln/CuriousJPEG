#include "dqt_decoder.hpp"
#include <iostream>

#include "data_reader.hpp"
#include "utility.hpp"

void DQTDecoder::Invoke(std::istream &aStream, Context& aContext)
{
    const auto size = DataReader::readNumber<uint16_t>(aStream);
    printSectionDescription("Define a Quantization Table", size);

    uint8_t tableElementSize=0;
    uint8_t tableIndex=0;

    const auto numBuffer = DataReader::readNumber<uint8_t>(aStream);

    // TODO: To the utility module, or search something standard
    tableElementSize = lowByte( numBuffer );
    tableIndex= highByte( numBuffer );

    if (aContext.DQT_Vector.size() <= tableIndex ) {
        aContext.DQT_Vector.resize(tableIndex + 1);
    }

    std::array<uint16_t, 64> buffer;

    if (tableElementSize==0)
    {
        std::array<uint8_t, 64> byteBuffer;
        DataReader::readBuffer(aStream, byteBuffer);
        //aStream.read(reinterpret_cast<std::istream::char_type*>(byteBuffer.data()),sizeof(byteBuffer));
        std::transform(std::begin(byteBuffer), std::end(byteBuffer),std::begin(buffer),[](uint8_t number){
            return uint16_t{number};
        });
    }
    else
    {
        DataReader::readBuffer(aStream, buffer);
        //aStream.read(reinterpret_cast<std::istream::char_type*>(buffer.data()),sizeof(buffer));
    }

    aContext.DQT_Vector[tableIndex] = CreateZigZagMatrix(buffer);
}
