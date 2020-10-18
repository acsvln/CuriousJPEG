#ifndef DATAREADER_HPP
#define DATAREADER_HPP

#include <boost/endian/conversion.hpp>
#include <istream>

class DataReader final {
  ~DataReader() = delete;

public:
  using StreamT = std::istream;
  using CharT = StreamT::char_type;
  using StreamSizeT = std::streamsize;

  template <class NumberT> static auto readNumber(StreamT &Stream) -> NumberT {
    NumberT Buffer;
    Stream.read(reinterpret_cast<CharT *>(&Buffer), sizeof(Buffer));
    return boost::endian::native_to_big(Buffer);
  }

  template <class StructT> static auto readSruct(StreamT &Stream, StructT &Struct) {
    Stream.read(reinterpret_cast<CharT *>(&Struct), sizeof(Struct));
  }

  template <class BufferT>
  static void readBuffer(StreamT &Stream, BufferT &Buffer) {
    static_assert(sizeof(CharT) == sizeof(typename BufferT::value_type));
    static_assert(sizeof(CharT *) == sizeof(typename BufferT::pointer));

    Stream.read(reinterpret_cast<CharT *>(Buffer.data()), Buffer.size());
  }

  static void skipChars(StreamT &Stream, const StreamSizeT Count);
};

#endif // DATAREADER_HPP
