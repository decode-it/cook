#ifndef HEADER_cook_chef_assistant_LinkOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_LinkOrder_hpp_ALREADY_INCLUDED

#include "cook/chef/Assistant.hpp"

namespace cook { namespace chef { namespace assistant {

class LinkLibrarySorter : public Assistant
{
public:
    std::string description() const override { return "Link library sorter"; }
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    Result process_(const Context & context, model::Snapshot & snapshot, ingredient::Collection<ingredient::File> & libraries) const;
};

} } }

#endif
