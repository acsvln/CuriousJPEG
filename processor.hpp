#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include <cstdint>
#include <memory>
#include <map>

class IDecoder;

class Processor final
{
public:
    using DecoderPtr = std::unique_ptr<IDecoder>;

    void RegisterDecoder(uint16_t aTag, DecoderPtr aDecoder);
    void Execute();

private:
    std::map<uint16_t, DecoderPtr> mDecoders;
};

#endif // PROCESSOR_HPP
