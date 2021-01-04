#include "decoder_processor.hpp"

#include <fstream>
#include <iostream>
#include <string>

#include "data_reader.hpp"
#include "decoder_context.hpp"
#include "definition.hpp"
#include "idecoder.hpp"
#include "utility.hpp"

void DecoderProcessor::Register(uint16_t Tag,
                                DecoderProcessor::DecoderPointer Decoder) {
  Decoders.emplace(Tag, std::move(Decoder));
}

// Here executing logic
void DecoderProcessor::Execute(const std::string &InPath,
                               const std::string &OutPath) {
  std::ifstream In;

  In.exceptions(In.exceptions() | std::ios::failbit);
  In.open(InPath, std::ios::in | std::ios::binary);

  std::cout << "File with path " << InPath << " opened successfully"
            << std::endl;

  DecoderContext Context;

  bool StartOfScanFlag = false;

  const auto messageEndOfImage = [] {
    std::cout << "End of image" << std::endl;
  };

  while (false == In.eof()) {
    if (true == StartOfScanFlag) {
      messageEndOfImage();
      break;
    }

    const auto Tag = DataReader::readNumber<uint16_t>(In);

    if (kJpgTagStartOfImage == Tag) {
      std::cout << "Start of image" << std::endl;
      continue;
    } else if (kJpgTagEndOfImage == Tag) {
      messageEndOfImage();
      continue;
    }

    std::cout << "Tag with code 0x" << std::hex << std::uppercase << Tag
              << " was readed" << std::endl;

    if (kJpgTagStartOfScan == Tag) {
      StartOfScanFlag = true;
    }

    if (Decoders.count(Tag) > 0) {
      Decoders[Tag]->Invoke(In, Context);
    } else {
      const auto Size = DataReader::readNumber<uint16_t>(In);
      const std::streamsize SkipByteCount = Size - sizeof(Size);

      std::cout << "Unsupported tag" << std::endl;
      std::cout << "Skipping " << SkipByteCount << " bytes" << std::endl;

      DataReader::skipChars(In, SkipByteCount);
    }
  }

  {
    using Channel = RGBBlock::Channel;
    const auto Width = Context.dct.Width;
    const auto Height = Context.dct.Height;

    Channel R(Height, Width), G(Height, Width), B(Height, Width);

    std::size_t OffsetY = 0;
    std::size_t OffsetX = 0;

    const auto PutBlock = [&]( const auto& RGB ){
        for (std::size_t I = 0; I < 16; ++I) {
          for (std::size_t J = 0; J < 16; ++J) {
              if ( OffsetY + I >= Height ) {
                  OffsetX += 16;
                  return;
              }

              if ( OffsetX + J >= Width ) {
                break;
              }

              R(OffsetY + I, OffsetX + J) = RGB.R(I, J);
              G(OffsetY + I, OffsetX + J) = RGB.G(I, J);
              B(OffsetY + I, OffsetX + J) = RGB.B(I, J);
          }
        }

        if ( OffsetX + 16 >= Width ) {
          OffsetY += 16;
          OffsetX = 0;
        } else {
          OffsetX += 16;
        }
    };

    for (const auto &RGB : Context.Image) {
        PutBlock(RGB);
    }

    saveRGBToImage(R, G, B, OutPath);
  }
}
