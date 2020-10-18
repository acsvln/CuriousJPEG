#include "bit_extractor.hpp"

#include <bitset>
#include <boost/assert.hpp>
#include <iostream>

#include "data_reader.hpp"

BitExtractor::BitExtractor(std::istream &Stream) : InputStream{Stream} {}

auto BitExtractor::nextNumber(const std::size_t BitCnt) -> uint16_t {
  BOOST_ASSERT_MSG(BitCnt > 0 &&
                       BitCnt <= std::numeric_limits<uint16_t>::digits,
                   "Bit count must be between 0 and 16");

  std::bitset<16> Result;

  for (std::size_t Index = BitCnt; Index != 0; --Index) {
    if (0 == BitCounter) {
      Bits = DataReader::readNumber<uint16_t>(InputStream);
      BitCounter = Bits.size();
    }

    BitCounter--;
    Result[Index - 1] = Bits[BitCounter];
  }

  return static_cast<uint16_t>(Result.to_ulong());
}
