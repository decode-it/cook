#ifndef HEADER_cook_agents_IncludePath_hpp_ALREADY_INCLUDED
#define HEADER_cook_agents_IncludePath_hpp_ALREADY_INCLUDED

#include "cook/agents/Interface.hpp"

namespace cook { namespace agents {

class IncludePath : public Interface
{
public:
    using SelectionFunction = std::function<bool (Language)>;

    IncludePath(const SelectionFunction selection_functor = SelectionFunction());

    bool process(const Context & context, model::Snapshot & snapshot, model::Snapshot & post) const override;

private:
    SelectionFunction selection_;
};

} }

#endif
