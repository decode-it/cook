#ifndef HEADER_cook_chef_assistant_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_Archiver_hpp_ALREADY_INCLUDED

#include "cook/chef/Assistant.hpp"
#include <set>

namespace cook { namespace chef { namespace assistant {

class Archiver : public Assistant
{
public:
    std::string description() const override { return "Archiver"; }
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    virtual ingredient::File construct_archive_file(const Context &context) const;
};

} } }



#endif
