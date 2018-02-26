#ifndef HEADER_cook_staff_IncludePathSetter_hpp_ALREADY_INCLUDED
#define HEADER_cook_staff_IncludePathSetter_hpp_ALREADY_INCLUDED

#include "cook/staff/Interface.hpp"

namespace cook { namespace staff {

class IncludePathSetter : public Interface
{
public:
    IncludePathSetter(Language language);

    Result process(const Context & context, model::Snapshot & snapshot) const override;

private:
    Language language_;
};

} }

#endif
