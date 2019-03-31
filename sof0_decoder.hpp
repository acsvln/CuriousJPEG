#ifndef SOF0_DECODER_HPP
#define SOF0_DECODER_HPP

#include "decoder.hpp"

class SOF0Decoder : public IDecoder
{
public:
    ~SOF0Decoder() final = default;

    // IDecoder interface
public:
    void Invoke(std::istream &aStream, Context& aContext) final;
};

#endif // SOF0_DECODER_HPP
