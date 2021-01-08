#ifndef SOF0_DECODER_HPP
#define SOF0_DECODER_HPP

#include "decoder_base.hpp"

class SOF0Decoder : public DecoderBase {
public:
  ~SOF0Decoder() final = default;
  SOF0Decoder();

  // IDecoder interface
public:
  void InvokeImpl(std::istream &Stream, DecoderContext &Context) final;
};

#endif // SOF0_DECODER_HPP
