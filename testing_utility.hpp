#ifndef TESTING_UTILITY_H
#define TESTING_UTILITY_H

#include <vector>
#include <algorithm>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include "context.hpp"
#include "idecoder.hpp"
#include "utility.hpp"

namespace boost::numeric::ublas {
auto operator==(matrix<uint16_t> const &left, matrix<uint16_t> const &right)
    -> bool;
// TODO: Maybe fix
auto operator==(matrix<uint8_t> const &left, matrix<uint8_t> const &right)
    -> bool;
auto operator==(matrix<int16_t> const &left, matrix<int16_t> const &right)
    -> bool;
} // namespace boost::numeric::ublas

template <class Container>
void invokeDecoderWithDataBuffer(IDecoder &Dec, Context &Ctx, Container &Buff) {
  namespace ios = boost::iostreams;

  std::vector<char> Buffer(Buff.size());
  std::transform(std::begin(Buff), std::end(Buff), std::begin(Buffer), TransformCaster<char>() );

  ios::basic_array_source<char> InputSource(Buffer.data(), Buffer.size());
  ios::stream<ios::basic_array_source<char>> InputStream(InputSource);
  Dec.Invoke(InputStream, Ctx);
}

template <class Decoder, class Container>
void invokeDecoderWithDataBuffer(Context &Ctx, Container &Buff) {
    Decoder Dec;
    invokeDecoderWithDataBuffer(Dec, Ctx, Buff);
}

#endif // TESTING_UTILITY_H
