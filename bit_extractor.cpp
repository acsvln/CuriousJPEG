#include "bit_extractor.hpp"

#include "data_reader.hpp"

#include <boost/assert.hpp>

#include <iostream>
#include <bitset>

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

auto BitExtractor::nextNumber16(std::size_t const BitCnt) -> uint16_t {
    BOOST_ASSERT_MSG(BitCnt > 0 && BitCnt <= 16,
                     "Bit count must be between 0 and 16");

    std::bitset<16> Result;

    for ( std::size_t Index = BitCnt; Index != 0 ; --Index ) {
        if (0 == Counter16) {
            //
        // TODO: Bitset
//          uint16_t t;
//          DataReader::readSruct( Stream, t );
//          Bits16 = t;
            Bits16 = DataReader::readNumber<uint16_t>(Stream);


          Counter16 = Bits16.size();
        }

        Counter16--;
        Result[Index - 1] = Bits16[Counter16];
    }

    return static_cast<uint16_t>(Result.to_ulong());
}
