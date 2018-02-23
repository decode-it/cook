#ifndef HEADER_cook_chai_Environment_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Environment_hpp_ALREADY_INCLUDED

#include "cook/model/Environment.hpp"
#include "cook/chai/UserData.hpp"

namespace cook { namespace chai {

class Environment : public model::Environment
{
public:
    Result set_variable(const std::string & name, const std::string & value) override;

private:
    UserData data_;
};

} }

#endif
