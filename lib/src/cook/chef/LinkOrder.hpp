#ifndef HEADER_cook_chef_LinkOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_LinkOrder_hpp_ALREADY_INCLUDED

#include "cook/chef/Interface.hpp"

namespace cook { namespace chef {

class LinkOrder : public Interface
{
public:
    using SelectionFunction = std::function<bool (Language)>;

    explicit LinkOrder(const SelectionFunction & selection_function = SelectionFunction());

    bool process(const Context & context, model::Snapshot & snapshot, model::Snapshot & post) const override;

private:
    bool process_(const Context & context, model::Snapshot & snapshot, ingredient::Collection<ingredient::File> & libraries) const;

    SelectionFunction selection_;
};

} }

#endif
