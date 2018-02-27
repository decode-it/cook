#ifndef HEADER_cook_souschef_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_souschef_Linker_hpp_ALREADY_INCLUDED

#include "cook/souschef/Interface.hpp"
#include <set>

namespace cook { namespace souschef {

class Linker : public Interface
{
public:
    std::string description() const override { return "Linker"; }
    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    virtual ingredient::File construct_archive_file(const Context &context) const;
};

} }



#endif
