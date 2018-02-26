#ifndef HEADER_cook_chef_assistant_Interface_hpp_ALREADY_DEFINED
#define HEADER_cook_chef_assistant_Interface_hpp_ALREADY_DEFINED

#include "cook/chef/assistant/Context.hpp"

namespace cook { namespace chef { namespace assistant {

class Interface
{
public:
    virtual ~Interface() {}

    virtual Result process(const Context & context, model::Snapshot & snapshot) const = 0;
};

} } }

#endif
