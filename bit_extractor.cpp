#include "bit_extractor.hpp"

#include <bitset>
#include <boost/assert.hpp>
#include <iostream>

#include "data_reader.hpp"

BitExtractor::BitExtractor(std::istream &Strm) : Stream{Strm} {}

auto BitExtractor::nextNumber(std::size_t const BitCnt) -> uint16_t {
  BOOST_ASSERT_MSG(BitCnt > 0 &&
                       BitCnt <= std::numeric_limits<uint16_t>::digits,
                   "Bit count must be between 0 and 16");

  std::bitset<16> Result;

  for (std::size_t Index = BitCnt; Index != 0; --Index) {
    if (0 == Counter) {
      Bits = DataReader::readNumber<uint16_t>(Stream);
      Counter = Bits.size();
    }

    Counter--;
    Result[Index - 1] = Bits[Counter];
  }

  return static_cast<uint16_t>(Result.to_ulong());
}
