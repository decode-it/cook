#include "cook/chef/LibraryPath.hpp"
#include "cook/chef/PathExtraction.hpp"
#include <set>

namespace cook { namespace chef {

LibraryPath::LibraryPath()
    : selection_([](cook::Language ) { return true;})
{

}

LibraryPath::LibraryPath(const SelectionFunction & selection_functor)
    : selection_(selection_functor)
{

}

Result LibraryPath::process(const Context & context, model::Snapshot & snapshot, model::Snapshot & /*post*/) const
{
    MSS_BEGIN(Result);

    auto selector = [&](const LanguageTypePair & key) -> std::optional<LanguageTypePair>
    {
        if (key.type == Type::Library && selection_(key.language))
            return LanguageTypePair(key.language, Type::LibraryPath);
        else
            return {};
    };

    MSS(add_derived_path(selector, snapshot, snapshot, context.recipe));

    MSS_END();
}


} }
