#ifndef HEADER_cook_agents_LinkOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_agents_LinkOrder_hpp_ALREADY_INCLUDED

#include "cook/agents/Interface.hpp"

namespace cook { namespace agents {

class LinkOrder : public Interface
{
public:
    using SelectionFunction = std::function<bool (Language)>;

    explicit LinkOrder(const SelectionFunction & selection_function = SelectionFunction());

    bool process(const Context & context, model::Snapshot & snapshot, model::Snapshot & post) const override;

private:
    bool process_(const Context & context, model::Snapshot & snapshot, property::Collection<property::File> & libraries) const;

    SelectionFunction selection_;
};

} }

#endif
