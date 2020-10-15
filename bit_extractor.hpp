#ifndef BITEXTRACTOR_HPP
#define BITEXTRACTOR_HPP

#include <bitset>
#include <istream>

class BitExtractor final {
public:
  explicit BitExtractor(std::istream &Stream);
  BitExtractor(BitExtractor const &) = delete;

  auto nextNumber(std::size_t const BitCnt = 1) -> uint16_t;

private:
  std::bitset<16> Bits;
  std::size_t Counter = 0;
  std::istream &Stream;
};

#endif // BITEXTRACTOR_HPP
