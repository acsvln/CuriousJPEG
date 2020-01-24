#ifndef DHT_DECODER_H
#define DHT_DECODER_H

#include "decoder.hpp"

class DHTDecoder : public IDecoder
{
public:
    ~DHTDecoder() final = default;

    // IDecoder interface
public:
    void Invoke(Context &aContext) final;
};

#endif // DHT_DECODER_H
