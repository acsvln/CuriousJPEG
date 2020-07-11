#ifndef DATAREADER_HPP
#define DATAREADER_HPP

#include <istream>
#include <string>
#include <type_traits>

#include <boost/endian/conversion.hpp>

class DataReader final {
  ~DataReader() = delete;

public:
  using StreamT = std::istream;
  using CharT = StreamT::char_type;
  using UCharT = typename std::make_unsigned<CharT>::type;
  using StreamSizeT = std::streamsize;

  template <class NumberT> static auto readNumber(StreamT &Stream) -> NumberT {
    NumberT Buffer;
    Stream.read(reinterpret_cast<CharT *>(&Buffer), sizeof(Buffer));
    return boost::endian::native_to_big(Buffer);
  }

  template <class Struc> static auto readSruct(StreamT &Stream, Struc &struc) {
    Stream.read(reinterpret_cast<CharT *>(&struc), sizeof(struc));
  }

  template <class BufferT>
  static void readBuffer(StreamT &Stream, BufferT &Buffer) {
    static_assert(sizeof(CharT) == sizeof(typename BufferT::value_type));
    static_assert(sizeof(CharT *) == sizeof(typename BufferT::pointer));

    Stream.read(reinterpret_cast<CharT *>(Buffer.data()), Buffer.size());
  }

  static auto getRawChar(StreamT &Stream) -> UCharT;
  static void skipChars(StreamT &Stream, StreamSizeT const Count);
};

#endif // DATAREADER_HPP
