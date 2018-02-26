#ifndef HEADER_cook_chef_assistant_LibraryPathSetter_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_LibraryPathSetter_hpp_ALREADY_INCLUDED

#include "cook/chef/assistant/Interface.hpp"

namespace cook { namespace chef { namespace assistant {

class LibraryPathSetter : public Interface
{
public:
    Result process(const Context & context, model::Snapshot & snapshot) const override;
};

} } }

#endif
