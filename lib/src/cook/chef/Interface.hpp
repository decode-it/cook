#ifndef HEADER_cook_chef_Interface_hpp_ALREADY_DEFINED
#define HEADER_cook_chef_Interface_hpp_ALREADY_DEFINED

#include "cook/chef/Context.hpp"

namespace cook { namespace chef {

class Interface
{
public:
    virtual ~Interface() {}

    virtual Result process(const Context & context, model::Snapshot & snapshot, model::Snapshot & post) const = 0;
};

} }

#endif
