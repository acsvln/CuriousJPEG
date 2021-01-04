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
      const auto Mask = std::bitset<16>{0x00FF};
      const auto RawBits = DataReader::readNumber<uint16_t>(InputStream);
      if ( 0xFF00 == RawBits ) {
        Bits = 0xFF;
        BitCounter = 8;
      } else if (  Mask == ( PrevBits & Mask ) ) {
          Bits = RawBits;
          BitCounter = 8;
      } else {
        Bits = RawBits;
        BitCounter = Bits.size();
      }
      PrevBits = RawBits;
    }

    BitCounter--;
    Result[Index - 1] = Bits[BitCounter];
  }

  return static_cast<uint16_t>(Result.to_ulong());
}
