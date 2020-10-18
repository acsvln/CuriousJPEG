#ifndef IDECODER_HPP
#define IDECODER_HPP

#include <fstream>

#include "decoder_context.hpp"

class IDecoder {
public:
  virtual ~IDecoder() = default;

  virtual void Invoke(std::istream &Stream, DecoderContext &Context) = 0;
};

#endif // IDECODER_HPP
