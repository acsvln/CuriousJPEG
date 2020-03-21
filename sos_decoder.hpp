#ifndef SOS_DECODER_HPP
#define SOS_DECODER_HPP

#include "decoder.hpp"

class SOSDecoder : public IDecoder
{
public:
    ~SOSDecoder() final = default;

    // IDecoder interface
public:
    void Invoke(std::istream &aStream, Context& aContext) final;
};

#endif // SOS_DECODER_HPP
