#include "exceptions.hpp"

#include <sstream>

namespace {

auto buildExceptionText(const std::string &Caption,
                        const std::string &Message) {
  std::stringstream SStream;
  SStream << Caption << ": " << Message;
  return SStream.str();
}

} // namespace

NotImplementedException::NotImplementedException(const std::string &Message)
    : std::logic_error(buildExceptionText("Not yet implemented", Message)){};

InvalidJPEGDataException::InvalidJPEGDataException(const std::string &Message)
    : std::runtime_error{buildExceptionText("Invalid JPEG Data", Message)} {};
