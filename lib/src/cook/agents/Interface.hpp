#ifndef HEADER_cook_agents_Interface_hpp_ALREADY_DEFINED
#define HEADER_cook_agents_Interface_hpp_ALREADY_DEFINED

#include "cook/agents/Context.hpp"

namespace cook { namespace agents {

class Interface
{
public:
    virtual ~Interface() {}

    virtual bool process(const Context & context, model::Snapshot & snapshot, model::Snapshot & post) const = 0;
};

} }

#endif
