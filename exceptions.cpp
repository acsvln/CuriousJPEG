#include "exceptions.hpp"

NotImplementedException::NotImplementedException()
    : std::logic_error("Function not yet implemented"){};
