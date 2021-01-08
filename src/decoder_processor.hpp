#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <cstdint>
#include <map>
#include <memory>

class IDecoder;

class DecoderProcessor final {
public:
  using DecoderPointer = std::unique_ptr<IDecoder>;

  void Register(uint16_t Tag, DecoderPointer Decoder);
  void Execute(const std::string &InPath, const std::string &OutPath);

private:
  std::map<uint16_t, DecoderPointer> Decoders;
};

#endif // PROCESSOR_HPP
