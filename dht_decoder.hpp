#ifndef DHT_DECODER_HPP
#define DHT_DECODER_HPP

#include "decoder.hpp"

class DHTDecoder : public Decoder {
public:
  ~DHTDecoder() final = default;
  DHTDecoder();

  // Decoder interface
public:
  void InvokeImpl(std::istream &Stream, Context &Ctx) final;
};

#endif // DHT_DECODER_HPP
