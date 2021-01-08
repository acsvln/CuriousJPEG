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
      std::bitset<16> const Mask_00FF{0x00FF};
      std::bitset<16> const Mask_FF00{0xFF00};
      const auto Size = Bits.size();
      const auto HalfSize = Bits.size() / 2;
      const auto RawBits = DataReader::readNumber<uint16_t>(InputStream);
      if (Mask_FF00 == RawBits) {
        Bits = RawBits >> HalfSize;
        BitCounter = HalfSize;
      } else if (Mask_00FF == (PreviousBits & Mask_00FF)) {
        Bits = RawBits;
        BitCounter = HalfSize;
      } else {
        Bits = RawBits;
        BitCounter = Size;
      }
      PreviousBits = RawBits;
    }

    BitCounter--;
    Result[Index - 1] = Bits[BitCounter];
  }

  return static_cast<uint16_t>(Result.to_ulong());
}
