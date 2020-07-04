#ifndef DHT_DECODER_H
#define DHT_DECODER_H

#include <array>
#include <sstream>
#include <string>

#include "decoder.hpp"
#include "utility.hpp"

std::shared_ptr<DHTNode> PushHuffValue(std::shared_ptr<DHTNode> parent,
                                       uint8_t level, uint8_t value);

class DHTDecoder : public IDecoder {
public:
  ~DHTDecoder() final = default;

  // IDecoder interface
public:
  void Invoke(std::istream &aStream, Context &aContext) final;
};

#endif // DHT_DECODER_H
