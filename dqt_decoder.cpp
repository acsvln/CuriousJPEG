#include "dqt_decoder.hpp"
#include <iostream>

void DQTDecoder::Invoke([[maybe_unused]]std::istream &aStream, [[maybe_unused]]Context& aContext)
{
     std::cout << "Invoking DQTDecoder" << std::endl;
}
