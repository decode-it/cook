#ifndef HEADER_cook_souschef_LibraryPathSetter_hpp_ALREADY_INCLUDED
#define HEADER_cook_souschef_LibraryPathSetter_hpp_ALREADY_INCLUDED

#include "cook/souschef/Interface.hpp"

namespace cook { namespace souschef {

class LibraryPathSetter : public Interface
{
public:
    std::string description() const override { return "Library path setter"; }
    Result process(const Context & context, model::Snapshot & snapshot) const override;
};

} }

#endif
