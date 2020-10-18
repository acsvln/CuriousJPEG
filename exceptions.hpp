#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

class NotImplementedException : public std::logic_error {
public:
  NotImplementedException(const std::string &Message);
};

class InvalidJPEGDataException : public std::runtime_error {
public:
  InvalidJPEGDataException(const std::string &Message);
};

#endif // EXCEPTIONS_HPP
