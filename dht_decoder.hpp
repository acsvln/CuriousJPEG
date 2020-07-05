#ifndef DHT_DECODER_H
#define DHT_DECODER_H

#include <array>
#include <sstream>
#include <string>

#include "decoder.hpp"
#include "utility.hpp"

class DHTDecoder : public IDecoder {
public:
  ~DHTDecoder() final = default;

  // IDecoder interface
public:
  void Invoke(std::istream &aStream, Context &aContext) final;
};

#endif // DHT_DECODER_H
