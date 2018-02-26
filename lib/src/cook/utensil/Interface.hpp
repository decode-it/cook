#ifndef HEADER_cook_utensil_Interface_hpp_ALREADY_DEFINED
#define HEADER_cook_utensil_Interface_hpp_ALREADY_DEFINED

#include "cook/utensil/Context.hpp"

namespace cook { namespace utensil {

class Interface
{
public:
    virtual ~Interface() {}

    virtual Result process(const Context & context, model::Snapshot & snapshot) const = 0;
};

} }

#endif
