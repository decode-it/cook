#ifndef HEADER_cook_utensil_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_utensil_Linker_hpp_ALREADY_INCLUDED

#include "cook/utensil/Interface.hpp"
#include <set>

namespace cook { namespace utensil {

class Linker : public Interface
{
public:
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    virtual ingredient::File construct_archive_file(const Context &context) const;
};

} }



#endif
