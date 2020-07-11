#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

class NotImplementedException : public std::logic_error {
public:
  NotImplementedException();
};

#endif // EXCEPTIONS_H
