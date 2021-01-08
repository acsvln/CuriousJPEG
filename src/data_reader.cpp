#include "data_reader.hpp"

void DataReader::skipChars(StreamT &Stream, const StreamSizeT Count) {
  Stream.ignore(Count);
}
