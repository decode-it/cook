#ifndef HEADER_cook_chef_LibraryPath_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_LibraryPath_hpp_ALREADY_INCLUDED

#include "cook/chef/Interface.hpp"

namespace cook { namespace chef {

class LibraryPath : public Interface
{
public:
    using SelectionFunction = std::function<bool (Language)>;

    LibraryPath();
    explicit LibraryPath(const SelectionFunction & selection_functor);

    Result process(const Context & context, model::Snapshot & snapshot, model::Snapshot & post) const override;

private:
    SelectionFunction selection_;
};

} }

#endif
