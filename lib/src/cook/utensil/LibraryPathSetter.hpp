#ifndef HEADER_cook_utensil_LibraryPathSetter_hpp_ALREADY_INCLUDED
#define HEADER_cook_utensil_LibraryPathSetter_hpp_ALREADY_INCLUDED

#include "cook/utensil/Interface.hpp"

namespace cook { namespace utensil {

class LibraryPathSetter : public Interface
{
public:
    Result process(const Context & context, model::Snapshot & snapshot) const override;
};

} }

#endif
