#ifndef HEADER_cook_staff_souschef_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_souschef_Linker_hpp_ALREADY_INCLUDED

#include "cook/staff/souschef/Interface.hpp"
#include <set>

namespace cook { namespace staff { namespace souschef {

class Linker : public Interface
{
public:
    std::string description() const override { return "Linker"; }
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    virtual ingredient::File construct_archive_file(const Context &context) const;
};

} } }



#endif
