#ifndef HEADER_cook_chef_Instruction_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_Instruction_hpp_ALREADY_INCLUDED

#include "cook/chef/assistant/Interface.hpp"
#include <list>
#include <vector>
#include <string>
#include <functional>

namespace cook { namespace chef {

struct Instruction
{
    using UtensilPtr = std::shared_ptr<assistant::Interface>;

    std::string name;
    std::string description;
    std::vector<UtensilPtr> steps;
};

using Instructions = std::list<Instruction>;

struct InstructionSet
{
    using DecisionFunction = std::function<bool (const model::Recipe *)>;

    Instructions instructions;
    DecisionFunction decision_function;
};

} }

#endif
