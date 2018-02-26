#ifndef HEADER_cook_chef_InstructionSet_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_InstructionSet_hpp_ALREADY_INCLUDED

#include "cook/chef/Assistant.hpp"
#include "cook/model/Recipe.hpp"
#include <functional>

namespace cook { namespace chef {

struct InstructionSet
{
    std::string name;
    std::function< bool (const model::Recipe *, std::string & )> can_cook;
    std::list<AssistantPtr> assistants;
};

} }

#endif
