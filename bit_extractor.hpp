#ifndef BITEXTRACTOR_HPP
#define BITEXTRACTOR_HPP

#include <istream>
#include <bitset>

class BitExtractor final {
public:
  explicit BitExtractor(std::istream &Stream);

  auto nextNumber(std::size_t const BitCnt = 1) -> uint8_t;
  auto nextNumber16(std::size_t const BitCnt = 1) -> uint16_t;

private:
  std::bitset<8> Bits;
  std::size_t Counter = 0;
  std::bitset<16> Bits16;
  std::size_t Counter16 = 0;
  std::istream &Stream;
};

#endif // BITEXTRACTOR_HPP