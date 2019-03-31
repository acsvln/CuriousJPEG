#include "dqt_decoder.hpp"
#include <iostream>
#include <fstream>
#include "utility.hpp"

void DQTDecoder::Invoke(std::istream& aStream, Context& aContext)
{
    std::cout << "Invoking DQTDecoder" << std::endl;

    // TODO: Fix that crap (Look processor.cpp)
    uint16_t size;
    aStream.read(reinterpret_cast<char*>(&size),sizeof(uint16_t));
    size = swap_endian(size);

    //==============================================

    uint8_t tableElementSize=0;
    uint8_t tableIndex=0;
    char temp; // TODO: remove char

    aStream.get(temp);

    tableElementSize = temp >> 4; // TODO: maybe we can look for some standard solutions
    tableIndex= temp & 0xF;

    if (tableElementSize==0) temp=8;
    else temp=16;

    Context::QuantTable table{8,8};


    aContext.quantTables.push_back(table);
}
