#include "exceptions.h"

NotImplementedException::NotImplementedException()
    : std::logic_error("Function not yet implemented"){};
