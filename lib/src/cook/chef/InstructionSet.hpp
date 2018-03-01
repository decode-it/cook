#ifndef HEADER_cook_staff_chef_InstructionSet_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_chef_InstructionSet_hpp_ALREADY_INCLUDED

#include "cook/souschef/Interface.hpp"
#include "cook/model/Recipe.hpp"
#include <functional>

namespace cook { namespace chef {

using AssistantPtr = std::shared_ptr<souschef::Interface>;

struct InstructionSet
{
    std::string name;
    std::function< bool (const model::Recipe *, std::string & )> can_cook;
    std::list<AssistantPtr> assistants;
};

} }

#endif
