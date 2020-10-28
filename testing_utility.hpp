#ifndef TESTING_UTILITY_HPP
#define TESTING_UTILITY_HPP

#include <algorithm>
#include <vector>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include "decoder_context.hpp"
#include "idecoder.hpp"
#include "utility.hpp"

template <typename T> using MatrixT = boost::numeric::ublas::matrix<T>;

namespace boost::numeric::ublas {
auto operator==(const matrix<uint16_t> &left, const matrix<uint16_t> &right)
    -> bool;
// TODO: Maybe fix
auto operator==(const matrix<uint8_t> &left, const matrix<uint8_t> &right)
    -> bool;
auto operator==(const matrix<int8_t> &left, const matrix<int8_t> &right)
    -> bool;
auto operator==(const matrix<int16_t> &left, const matrix<int16_t> &right)
    -> bool;
} // namespace boost::numeric::ublas

template <class Container>
auto charVectorForBuffer(Container &Buff) -> std::vector<char> {
  std::vector<char> Buffer(Buff.size());
  std::transform(std::begin(Buff), std::end(Buff), std::begin(Buffer),
                 TransformCaster<char>());
  return Buffer;
}

template <class Container>
void invokeDecoderWithDataBuffer(IDecoder &Decoder, DecoderContext &Context,
                                 Container &Buff) {
  namespace ios = boost::iostreams;

  std::vector<char> Buffer(Buff.size());
  std::transform(std::begin(Buff), std::end(Buff), std::begin(Buffer),
                 TransformCaster<char>());

  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
  Decoder.Invoke(InputStream, Context);
}

template <class Decoder, class Container>
void invokeDecoderWithDataBuffer(DecoderContext &Context, Container &Buff) {
  Decoder DecoderInstance;
  invokeDecoderWithDataBuffer(DecoderInstance, Context, Buff);
}

template <class Type>
void printMatrix(const MatrixT<Type> &Matrix, const bool HexMode = false) {
  const auto RowCount = Matrix.size1();
  const auto ColCount = Matrix.size2();

  std::cout << RowCount << 'x' << ColCount << ':' << std::endl;

  for (std::size_t Row = 0; Row < RowCount; ++Row) {
    std::cout << '\t';
    for (std::size_t Col = 0; Col < ColCount; ++Col) {
      if (true == HexMode) {
        std::cout << std::hex << std::uppercase
                  << static_cast<unsigned>(Matrix(Row, Col)) << '\t';
      } else {
        std::cout << static_cast<signed>(Matrix(Row, Col)) << '\t';
      }
    }
    std::cout << std::endl;
  }
}

void printVector(const std::array<int16_t, 64> &vector);

#endif // TESTING_UTILITY_HPP
