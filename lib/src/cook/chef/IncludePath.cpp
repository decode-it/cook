#include "cook/chef/IncludePath.hpp"
#include "cook/chef/PathExtraction.hpp"
#include <set>

namespace cook { namespace chef {

IncludePath::IncludePath()
    : selection_([](Language) { return true; })
{
}

IncludePath::IncludePath(const SelectionFunction selection_functor)
    : selection_(selection_functor)
{

}


Result IncludePath::process(const Context & context, model::Snapshot & snapshot, model::Snapshot & /*post*/) const
{
    MSS_BEGIN(Result);

    auto selector = [&](const LanguageTypePair & key) -> std::optional<LanguageTypePair>
    {
        if (key.type == Type::Header && selection_(key.language))
            return LanguageTypePair(key.language, Type::IncludePath);
        else
            return {};
    };

    MSS(add_derived_path(selector, snapshot, snapshot, context.recipe));

    MSS_END();
}

} }

