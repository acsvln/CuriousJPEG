#include <boost/program_options.hpp>
#include <iostream>
#include <memory>

#include "decoder_processor.hpp"
#include "definition.hpp"

#include "dht_decoder.hpp"
#include "dqt_decoder.hpp"
#include "sof0_decoder.hpp"
#include "sos_decoder.hpp"

bool processCommandLine(int argc, char **argv, std::string &ImagePath,
                        std::string &OutputPath) try {
  namespace po = boost::program_options;
  po::options_description MainDescription("All options");

  // clang-format off
  MainDescription.add_options()
    ("help,h", "Show help")
    ("input,i", po::value<std::string>(&ImagePath)->required(),"Input image path")
    ("output,o",po::value<std::string>(&OutputPath)->required(),"Output image path");
  // clang-format on

  po::variables_map VM;
  po::parsed_options Parsed =
      po::command_line_parser(argc, argv).options(MainDescription).run();
  po::store(Parsed, VM);

  if (VM.count("help")) {
    std::cout << MainDescription << std::endl;
    return true;
  }

  po::notify(VM);

  return true;
} catch (const std::exception &e) {
  std::cerr << "Command line processing error: " << e.what() << "\n";
  return false;
}

int main(int argc, char *argv[]) {
  try {
    std::string ImagePath;
    std::string OutputPath;

    if (!processCommandLine(argc, argv, ImagePath, OutputPath)) {
      return EXIT_FAILURE;
    }

    DecoderProcessor Processor;

    Processor.Register(kJpgTagQuantTableDef, std::make_unique<DQTDecoder>());
    Processor.Register(kJpgTagHuffmanTableDef, std::make_unique<DHTDecoder>());
    Processor.Register(kJpgTagHuffBaselineDCT, std::make_unique<SOF0Decoder>());
    Processor.Register(kJpgTagStartOfScan, std::make_unique<SOSDecoder>());

    Processor.Execute(ImagePath, OutputPath);

  } catch (std::ios_base::failure &E) {
    std::cerr << "Can not open file: " << E.what() << '\n';
    return EXIT_FAILURE;
  } catch (std::exception &E) {
    std::cerr << E.what() << '\n';
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "Unknown error occured!" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
