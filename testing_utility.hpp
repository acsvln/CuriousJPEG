#ifndef TESTING_UTILITY_HPP
#define TESTING_UTILITY_HPP

#include <algorithm>
#include <vector>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include "context.hpp"
#include "idecoder.hpp"
#include "utility.hpp"

template<typename T>
using MatrixT = boost::numeric::ublas::matrix<T>;

namespace boost::numeric::ublas {
auto operator==(matrix<uint16_t> const &left, matrix<uint16_t> const &right)
    -> bool;
// TODO: Maybe fix
auto operator==(matrix<uint8_t> const &left, matrix<uint8_t> const &right)
    -> bool;
auto operator==(matrix<int8_t> const &left, matrix<int8_t> const &right)
    -> bool;
auto operator==(matrix<int16_t> const &left, matrix<int16_t> const &right)
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
void invokeDecoderWithDataBuffer(IDecoder &Dec, Context &Ctx, Container &Buff) {
  namespace ios = boost::iostreams;

  std::vector<char> Buffer(Buff.size());
  std::transform(std::begin(Buff), std::end(Buff), std::begin(Buffer),
                 TransformCaster<char>());

  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
  Dec.Invoke(InputStream, Ctx);
}

template <class Decoder, class Container>
void invokeDecoderWithDataBuffer(Context &Ctx, Container &Buff) {
  Decoder Dec;
  invokeDecoderWithDataBuffer(Dec, Ctx, Buff);
}

template <class Type>
void printMatrix(MatrixT<Type> const& Matrix, bool const HexMode = false) {
  const auto RowCount =  Matrix.size1();
  const auto ColCount =  Matrix.size2();

  std::cout << RowCount << 'x' << ColCount << ':' << std::endl;

  for (std::size_t Row = 0; Row  < RowCount; ++Row ) {
    std::cout << '\t';
    for (std::size_t Col = 0; Col < ColCount; ++Col) {
        if ( true == HexMode ) {
            std::cout << std::hex << std::uppercase << static_cast<unsigned>( Matrix(Row, Col) ) << '\t';
        } else {
            std::cout << static_cast<signed>( Matrix(Row, Col) ) << '\t';
        }
    }
    std::cout << std::endl;
  }
}

void saveRGBToImage(
        MatrixT<uint8_t> const& R,
        MatrixT<uint8_t> const& G,
        MatrixT<uint8_t> const& B,
        std::string const& path );

#endif // TESTING_UTILITY_HPP
