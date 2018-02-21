#ifndef HEADER_cook_chai_Environment_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Environment_hpp_ALREADY_INCLUDED

#include "cook/model/Environment.hpp"
#include "cook/chai/UserData.hpp"

namespace cook { namespace chai {

class Environment : public model::Environment
{
public:
    bool set_variables(const std::list<Variable> & variables) override;

private:
    UserData data_;
};

} }

#endif
