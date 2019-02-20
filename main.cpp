#include <iostream>
#include <memory>

#include "definition.hpp"
#include "dqt_decoder.hpp"
#include "sof0_decoder.hpp"
#include "processor.hpp"

int main([[maybe_unused]]int argc, [[maybe_unused]]char* argv[])
{
    try {
        Processor processor;

        processor.RegisterDecoder(kJpgTagQuantTableDef, std::make_unique<DQTDecoder>());
        processor.RegisterDecoder(kJpgTagHuffBaselineDCT, std::make_unique<SOF0Decoder>());

        processor.Execute();

    } catch (...) {
        std::cout << "Error occured!" << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
