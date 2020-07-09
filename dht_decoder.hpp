#ifndef DHT_DECODER_H
#define DHT_DECODER_H

#include <array>
#include <sstream>
#include <string>

#include "decoder.hpp"
#include "utility.hpp"

class DHTDecoder : public Decoder {
public:
  ~DHTDecoder() final = default;
DHTDecoder();

  // Decoder interface
public:
  void InvokeImpl(std::istream &aStream, Context &aContext) final;
};

#endif // DHT_DECODER_H
