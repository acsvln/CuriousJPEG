#ifndef DECODER_HPP
#define DECODER_HPP

#include "idecoder.hpp"

class Decoder : public IDecoder {
protected:
  Decoder(std::string const &Header);

public:
  ~Decoder() override = default;

  void Invoke(std::istream &Stream, Context &Ctx) final;

private:
  virtual void InvokeImpl(std::istream &Stream, Context &Ctx) = 0;

private:
  std::string const DecoderHeader;
};

#endif // DECODER_HPP
