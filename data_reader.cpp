#include "data_reader.hpp"

void DataReader::skipChars(StreamT &Stream, StreamSizeT const Count) {
  Stream.ignore(Count);
}
