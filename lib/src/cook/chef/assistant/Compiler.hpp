#ifndef HEADER_cook_chef_assistant_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_assistant_Compiler_hpp_ALREADY_INCLUDED

#include "cook/chef/assistant/Interface.hpp"

namespace cook { namespace chef { namespace assistant {

class Compiler : public Interface
{
public:
    explicit Compiler(Language language);

    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    virtual ingredient::File construct_object_file(const ingredient::File & source, const Context &context) const;

    Language language_;

};

} } }



#endif
