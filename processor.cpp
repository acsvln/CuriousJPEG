#include "processor.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <boost/endian/conversion.hpp>

#include "decoder.hpp"
#include "definition.hpp"
#include "utility.hpp"

namespace {

namespace be = boost::endian;

template<class NumberType, class StreamType>
NumberType ReadNumberFromStream( StreamType& aStream )
{
    NumberType buffer;
    aStream.read(reinterpret_cast<char*>(&buffer),sizeof(buffer));
    return be::native_to_big(buffer);
}

}

void Processor::RegisterDecoder(uint16_t aTag, Processor::DecoderPtr aDecoder)
{
    mDecoders.emplace(aTag, std::move(aDecoder));
}

// Here executing logic
void Processor::Execute(const std::string& aPath)
{
    std::ifstream in;

    in.exceptions(in.exceptions() | std::ios::failbit);
    in.open(aPath, std::ios::in | std::ios::binary);

    std::cout << "File with path " << aPath << " opened successfully" << std::endl;

    Context context;

    while (in) {
        const auto tag = ReadNumberFromStream<uint16_t>(in);

        std::cout << "Tag with code 0x" << std::hex << std::uppercase << tag << " was readed" << std::endl;

        if (kJpgTagStartOfImage == tag) {
            std::cout << "Start of image" << std::endl;
            continue;
        }
        if (kJpgTagEndOfImage == tag) {
            std::cout << "End of image" << std::endl;
            break;
        }

        const auto size = ReadNumberFromStream<uint16_t>(in);

        std::cout << "Segment size: " << size << " bytes" << std::endl;

        if (mDecoders.count(tag) > 0) {
            mDecoders[tag]->Invoke(context);
        } else {
            std::streamsize skipByteCount = size - sizeof(size);

            std::cout << "Unknown tag!" << std::endl;
            std::cout << "Skipping " << skipByteCount << " bytes"  << std::endl;

            in.ignore(skipByteCount);
        }
    }
}
