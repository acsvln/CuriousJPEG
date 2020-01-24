#include "sof0_decoder.hpp"
#include <iostream>

void SOF0Decoder::Invoke([[maybe_unused]]std::istream &aStream, [[maybe_unused]]Context& aContext)
{
    std::cout << "Invoking SOF0Decoder" << std::endl;
}
