#ifndef HEADER_cook_process_chef_InstructionSet_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_chef_InstructionSet_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"
#include "cook/model/Recipe.hpp"
#include <functional>

namespace cook { namespace process { namespace chef {

using AssistantPtr = std::shared_ptr<souschef::Interface>;

struct InstructionSet
{
    std::string name;
    std::function< bool (const model::Recipe *, std::string & )> can_cook;
    std::list<AssistantPtr> assistants;
};

} } }

#endif