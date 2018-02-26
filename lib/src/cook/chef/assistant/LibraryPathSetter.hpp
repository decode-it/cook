#ifndef HEADER_cook_chef_assistant_LibraryPathSetter_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_LibraryPathSetter_hpp_ALREADY_INCLUDED

#include "cook/chef/Assistant.hpp"

namespace cook { namespace chef { namespace assistant {

class LibraryPathSetter : public Assistant
{
public:
    std::string description() const override { return "Library path setter"; }
    Result process(const Context & context, model::Snapshot & snapshot) const override;
};

} } }

#endif
