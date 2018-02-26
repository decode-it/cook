#ifndef HEADER_cook_staff_LibraryPathSetter_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_LibraryPathSetter_hpp_ALREADY_INCLUDED

#include "cook/staff/Interface.hpp"

namespace cook { namespace staff {

class LibraryPathSetter : public Interface
{
public:
    Result process(const Context & context, model::Snapshot & snapshot) const override;
};

} }

#endif
