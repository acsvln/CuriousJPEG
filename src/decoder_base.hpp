#ifndef DECODER_BASE_HPP
#define DECODER_BASE_HPP

#include "idecoder.hpp"

class DecoderBase : public IDecoder {
protected:
  DecoderBase(const std::string &Header);

public:
  ~DecoderBase() override = default;

  void Invoke(std::istream &Stream, DecoderContext &Context) final;

private:
  // SOSDecoder use own section size property handling
  friend class SOSDecoder;
  static void echoSegmentCaption(const std::string &Header,
                                 const uint16_t Size);

private:
  virtual void InvokeImpl(std::istream &Stream, DecoderContext &Context) = 0;

private:
  const std::string DecoderHeader;
};

#endif // DECODER_BASE_HPP
