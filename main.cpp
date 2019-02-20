#include <iostream>
#include <memory>

#include "definition.hpp"
#include "dqt_decoder.hpp"
#include "sof0_decoder.hpp"
#include "processor.hpp"

int main(int argc, char* argv[])
{
    try {
        if ( argc < 2) {
            std::cout << "Image path required";
            return EXIT_FAILURE;
        }

        std::string path{argv[1]};

        Processor processor;

        processor.RegisterDecoder(kJpgTagQuantTableDef, std::make_unique<DQTDecoder>());
        processor.RegisterDecoder(kJpgTagHuffBaselineDCT, std::make_unique<SOF0Decoder>());

        processor.Execute(path);

    }
    catch (std::ios_base::failure& e) {
      std::cerr << "Can not open file: " << e.what() << '\n';

      return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Unknown error occured!" << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
