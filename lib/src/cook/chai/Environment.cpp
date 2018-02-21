#include "cook/chai/Environment.hpp"

namespace cook { namespace chai {

bool Environment::set_variables(const std::list<Variable> & variables)
{
    for(const auto & v : variables)
        data_.set_variable(v.first, v.second);

    return true;
}

} }
