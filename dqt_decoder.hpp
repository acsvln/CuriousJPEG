#ifndef DQT_DECODER_HPP
#define DQT_DECODER_HPP

#include "decoder.hpp"

class DQTDecoder : public Decoder
{
public:
    ~DQTDecoder() final = default;
    DQTDecoder();

private:
  void InvokeImpl(std::istream &Stream, Context &Ctx);
};

#endif // DQT_DECODER_HPP
