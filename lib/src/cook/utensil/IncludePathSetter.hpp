#ifndef HEADER_cook_utensil_IncludePathSetter_hpp_ALREADY_INCLUDED
#define HEADER_cook_utensil_IncludePathSetter_hpp_ALREADY_INCLUDED

#include "cook/utensil/Interface.hpp"

namespace cook { namespace utensil {

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
