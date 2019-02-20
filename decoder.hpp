#ifndef DECODER_HPP
#define DECODER_HPP

#include "context.hpp"

class IDecoder
{
public:
    virtual ~IDecoder() = default;

    virtual void Invoke(Context& aContext) = 0;
};

#endif // DECODER_HPP
