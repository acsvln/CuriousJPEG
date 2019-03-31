#ifndef DQT_DECODER_HPP
#define DQT_DECODER_HPP

#include "decoder.hpp"

class DQTDecoder : public IDecoder
{
public:
    ~DQTDecoder() final = default;

    // IDecoder interface
public:
    void Invoke(std::istream &aStream, Context& aContext) final;
};

#endif // DQT_DECODER_HPP
