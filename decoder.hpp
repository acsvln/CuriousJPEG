#ifndef DECODER_HPP
#define DECODER_HPP

#include <fstream>

#include "context.hpp"

class IDecoder
{
public:
    virtual ~IDecoder() = default;

    virtual void Invoke(std::istream& aStream, Context& aContext) = 0;
};

#endif // DECODER_HPP
