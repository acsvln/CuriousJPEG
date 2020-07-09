#ifndef SOF0_DECODER_HPP
#define SOF0_DECODER_HPP

#include "decoder.hpp"

class SOF0Decoder : public Decoder {
public:
  ~SOF0Decoder() final = default;
  SOF0Decoder();

  // IDecoder interface
public:
  void InvokeImpl(std::istream &aStream, Context &aContext) final;
};

#endif // SOF0_DECODER_HPP
