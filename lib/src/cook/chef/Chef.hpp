#ifndef HEADER_cook_chef_Chef_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_Chef_hpp_ALREADY_INCLUDED

#include "cook/chef/InstructionSet.hpp"
#include "cook/model/Menu.hpp"
#include "cook/Kitchen.hpp"
#include <map>

namespace cook { namespace chef {

class Chef
{
public:
    virtual ~Chef() {}

    virtual Result initialize();

    template <typename InstructionSet_>
    void add_instruction_set(unsigned int priority, InstructionSet_ && instruction_set)
    {
        instruction_set_priority_map_.emplace(priority, std::forward<InstructionSet_>(instruction_set));
    }

    // I know it should mets en place, but this corresponds to the cooking term :)
    Result mis_en_place(Kitchen & kitchen, const model::Menu & menu);

    Result find_instruction_set(const InstructionSet *& result, model::Recipe * recipe) const;
    Result prepare_context(Context & context, model::Recipe * recipe, const model::Menu & menu, model::Environment * environment);

private:
    Result mis_en_place_(Kitchen & kitchen, const Context & context, const InstructionSet &instruction_set) const;

    std::multimap<unsigned int, InstructionSet> instruction_set_priority_map_;
};


} }

#endif
