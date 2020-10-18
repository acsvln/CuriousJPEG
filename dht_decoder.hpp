#ifndef DHT_DECODER_HPP
#define DHT_DECODER_HPP

#include "decoder_base.hpp"

class DHTDecoder : public DecoderBase {
public:
  ~DHTDecoder() final = default;
  DHTDecoder();

  // Decoder interface
public:
  void InvokeImpl(std::istream &Stream, DecoderContext &Context) final;
};

#endif // DHT_DECODER_HPP
