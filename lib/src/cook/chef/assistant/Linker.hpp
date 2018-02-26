#ifndef HEADER_cook_chef_assistant_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_Linker_hpp_ALREADY_INCLUDED

#include "cook/chef/Assistant.hpp"
#include <set>

namespace cook { namespace chef { namespace assistant {

class Linker : public Assistant
{
public:
    std::string description() const override { return "Linker"; }
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    virtual ingredient::File construct_archive_file(const Context &context) const;
};

} } }



#endif
