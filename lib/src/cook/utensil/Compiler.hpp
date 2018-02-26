#ifndef HEADER_cook_utensil_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_utensil_Compiler_hpp_ALREADY_INCLUDED

#include "cook/utensil/Interface.hpp"

namespace cook { namespace utensil {

class Compiler : public Interface
{
public:
    explicit Compiler(Language language);

    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    virtual ingredient::File construct_object_file(const ingredient::File & source, const Context &context) const;

    Language language_;

};

} }



#endif
