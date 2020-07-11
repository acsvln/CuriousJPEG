#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <stdexcept>

class NotImplementedException : public std::logic_error {
public:
  NotImplementedException();
};

#endif // EXCEPTIONS_HPP
