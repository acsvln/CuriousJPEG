#include "data_reader.hpp"

auto DataReader::getRawChar(StreamT &Stream) -> UCharT {
    return static_cast<UCharT>(Stream.get());
}

void DataReader::skipChars(StreamT &Stream, StreamSizeT const Count) {
  Stream.ignore(Count);
}
