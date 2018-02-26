#ifndef HEADER_cook_chef_assistant_IncludePathSetter_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_IncludePathSetter_hpp_ALREADY_INCLUDED

#include "cook/chef/assistant/Interface.hpp"

namespace cook { namespace chef { namespace assistant {

class IncludePathSetter : public Interface
{
public:
    IncludePathSetter(Language language);

    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    Language language_;
};

} } }

#endif
