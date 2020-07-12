#include "bit_extractor.hpp"

#include <boost/assert.hpp>

BitExtractor::BitExtractor( std::istream &Strm )
    : Stream{ Strm }
{}

auto BitExtractor::nextNumber(std::size_t const BitCnt) -> uint8_t {
  BOOST_ASSERT_MSG(BitCnt > 0 && BitCnt <= 8,
                   "Bit count must be between 0 and 8");

  std::bitset<8> Result;

  for ( std::size_t Index = BitCnt; Index != 0 ; --Index ) {
      if (0 == Counter) {
        Bits = static_cast<unsigned>(Stream.get());
        Counter = Bits.size();
      }
      Counter--;
      Result[Index - 1] = Bits[Counter];
  }
  return static_cast<uint8_t>(Result.to_ulong());
}
