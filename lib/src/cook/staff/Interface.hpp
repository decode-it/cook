#ifndef HEADER_cook_staff_Interface_hpp_ALREADY_DEFINED
#define HEADER_cook_staff_Interface_hpp_ALREADY_DEFINED

#include "cook/staff/Context.hpp"

namespace cook { namespace staff {

class Interface
{
public:
    virtual ~Interface() {}

    virtual Result process(const Context & context, model::Snapshot & snapshot) const = 0;
};

} }

#endif
