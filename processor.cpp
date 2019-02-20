#include "processor.hpp"
#include "decoder.hpp"
#include "definition.hpp"

void Processor::RegisterDecoder(uint16_t aTag, Processor::DecoderPtr aDecoder)
{
    mDecoders.emplace(aTag, std::move(aDecoder));
}

void Processor::Execute()
{
    Context context;

    // Here executing logic
    mDecoders[kJpgTagQuantTableDef]->Invoke(context);
    mDecoders[kJpgTagHuffBaselineDCT]->Invoke(context);
}
