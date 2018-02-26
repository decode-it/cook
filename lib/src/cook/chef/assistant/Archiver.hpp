#ifndef HEADER_cook_chef_assistant_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_Archiver_hpp_ALREADY_INCLUDED

#include "cook/chef/assistant/Interface.hpp"
#include <set>

namespace cook { namespace chef { namespace assistant {

class Archiver : public Interface
{
public:
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    virtual ingredient::File construct_archive_file(const Context &context) const;
};

} } }



#endif
