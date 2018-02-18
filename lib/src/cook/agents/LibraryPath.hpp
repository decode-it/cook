#ifndef HEADER_cook_agents_LibraryPath_hpp_ALREADY_INCLUDED
#define HEADER_cook_agents_LibraryPath_hpp_ALREADY_INCLUDED

#include "cook/agents/Interface.hpp"

namespace cook { namespace agents {

class LibraryPath : public Interface
{
public:
    using SelectionFunctor = std::function<bool (Language)>;

    LibraryPath(const SelectionFunctor & selection_functor = SelectionFunctor());

    bool process(const Context & context, model::Snapshot & snapshot, model::Snapshot & post) const override;

private:
    SelectionFunctor selection_;
};

} }

#endif
