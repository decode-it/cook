#ifndef HEADER_cook_chef_assistant_LinkOrder_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_LinkOrder_hpp_ALREADY_INCLUDED

#include "cook/chef/assistant/Interface.hpp"

namespace cook { namespace chef { namespace assistant {

class LinkLibrarySorter : public Interface
{
public:
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    Result process_(const Context & context, model::Snapshot & snapshot, ingredient::Collection<ingredient::File> & libraries) const;
};

} } }

#endif
