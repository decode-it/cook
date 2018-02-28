#ifndef HEADER_cook_staff_chef_Chef_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_chef_Chef_hpp_ALREADY_INCLUDED

#include "cook/staff/chef/InstructionSet.hpp"
#include "cook/Menu.hpp"
#include "cook/Context.hpp"
#include <map>

namespace cook { namespace staff { namespace chef {

class Chef
{
public:
    virtual ~Chef() {}

    virtual Result initialize() = 0;

    template <typename InstructionSet_>
    void add_instruction_set(unsigned int priority, InstructionSet_ && instruction_set)
    {
        instruction_set_priority_map_.emplace(priority, std::forward<InstructionSet_>(instruction_set));
    }

    // I know it should mets en place, but this corresponds to the cooking term :)
    Result mis_en_place(Context & kitchen);

    Result find_instruction_set(const InstructionSet *& result, model::Recipe * recipe) const;
    Result prepare_context(souschef::Context & context, model::Recipe * recipe, const Menu & menu, const model::Dirs *dirs);

private:
    Result mis_en_place_(Context & kitchen, const souschef::Context & context, const InstructionSet &instruction_set) const;

    std::multimap<unsigned int, InstructionSet> instruction_set_priority_map_;

};


} } }

#endif