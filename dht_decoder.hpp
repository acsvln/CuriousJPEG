#ifndef DHT_DECODER_H
#define DHT_DECODER_H

#include "decoder.hpp"

class DHTDecoder : public Decoder {
public:
  ~DHTDecoder() final = default;
  DHTDecoder();

  // Decoder interface
public:
  void InvokeImpl(std::istream &Stream, Context &Ctx) final;
};

#endif // DHT_DECODER_H
