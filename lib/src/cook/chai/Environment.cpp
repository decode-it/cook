#include "cook/chai/Environment.hpp"

namespace cook { namespace chai {

void Environment::copy_from(const Environment & rhs)
{
    *this = rhs;
    data_ = rhs.data_.clone();
}

bool Environment::set_variables(const std::list<Variable> & variables)
{
    for(const auto & v : variables)
        data_.set_variable(v.first, v.second);

    return true;
}

bool Environment::is_equal_(const model::Environment * env) const
{
    const chai::Environment * rhs = dynamic_cast<const chai::Environment *>(env);
    if (!rhs)
        return false;

    return data_ == rhs->data_;
}

} }
