#ifndef HEADER_cook_chef_Archive_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_Archive_hpp_ALREADY_INCLUDED

#include "cook/chef/Interface.hpp"

namespace cook { namespace chef {

class Archive : public Interface
{
public:
    Result process(const Context & context, model::Snapshot & snapshot, model::Snapshot & post) const override;
};

} }



#endif
