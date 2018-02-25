#ifndef HEADER_cook_chef_IncludePath_hpp_ALREADY_INCLUDED
#define HEADER_cook_chef_IncludePath_hpp_ALREADY_INCLUDED

#include "cook/chef/Interface.hpp"

namespace cook { namespace chef {

class IncludePath : public Interface
{
public:
    using SelectionFunction = std::function<bool (Language)>;

    IncludePath();
    explicit IncludePath(const SelectionFunction selection_functor);

    Result process(const Context & context, model::Snapshot & snapshot, model::Snapshot & post) const override;

private:
    SelectionFunction selection_;
};

} }

#endif
