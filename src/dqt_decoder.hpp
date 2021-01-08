#ifndef DQT_DECODER_HPP
#define DQT_DECODER_HPP

#include "decoder_base.hpp"

class DQTDecoder : public DecoderBase {
public:
  ~DQTDecoder() final = default;
  DQTDecoder();

private:
  void InvokeImpl(std::istream &Stream, DecoderContext &Context);
};

#endif // DQT_DECODER_HPP
