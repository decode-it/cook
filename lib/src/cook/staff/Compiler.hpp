#ifndef HEADER_cook_staff_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_Compiler_hpp_ALREADY_INCLUDED

#include "cook/staff/Interface.hpp"

namespace cook { namespace staff {

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
