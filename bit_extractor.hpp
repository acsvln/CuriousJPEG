#ifndef BITEXTRACTOR_HPP
#define BITEXTRACTOR_HPP

#include <bitset>
#include <istream>

class BitExtractor final {
public:
  explicit BitExtractor(std::istream &Stream);
  BitExtractor(const BitExtractor &) = delete;

  auto nextNumber(const std::size_t BitCnt = 1) -> uint16_t;

private:
  std::bitset<16> Bits;
  std::size_t BitCounter = 0;
  std::bitset<16> PrevBits;
  std::istream &InputStream;
};

#endif // BITEXTRACTOR_HPP
