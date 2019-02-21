#include "processor.hpp"

#include <fstream>
#include <iostream>

#include "decoder.hpp"
#include "definition.hpp"
#include "utility.hpp"

void Processor::RegisterDecoder(uint16_t aTag, Processor::DecoderPtr aDecoder)
{
    mDecoders.emplace(aTag, std::move(aDecoder));
}

// Here executing logic
void Processor::Execute(const std::string& path)
{
    std::ifstream in{};
    in.exceptions(in.exceptions() | std::ios::failbit);

    in.open(path, std::ios::in | std::ios::binary);

    std::cout << "File with path " << path << " opened successfully" << std::endl;

    Context context;
    // TODO: remove creepy swap_endians
    while (in) {
        uint16_t tag;

        // TODO: Here must be
        // in >> tag;
        // We must fix that creepy code

        in.read(reinterpret_cast<char*>(&tag),sizeof(uint16_t));

        tag = swap_endian(tag);

        std::cout << "Tag with code " << std::hex << tag << "was readed" << std::endl;

        if (kJpgTagStartOfImage == tag) {
            continue;
        }

        if (kJpgTagEndOfImage == tag) {
            break;
        }

        if (mDecoders.count(tag) > 0) {
            mDecoders[tag]->Invoke(in, context);
        } else {
            uint16_t size;

            in.read(reinterpret_cast<char*>(&size),sizeof(uint16_t));

            size = swap_endian(size);

            std::cout << "Segment size:" << size << std::endl;

            std::cout << "Unknown tag!" << std::endl;
            in.ignore(size - 2); // WATT? 2????
        }
    }
}
