#include "cook/chef/DependentPropagation.hpp"

namespace cook { namespace chef {

namespace  {

template <typename Tag>
Result merge_(const model::Snapshot & src_snapshot, model::Snapshot & dst_snapshot, const DependentPropagation::SelectionFunction & selection, Tag tag)
{
    MSS_BEGIN(Result);

    const auto & src_ingredients = src_snapshot.ingredients(tag);
    auto & dst_ingredients = dst_snapshot.ingredients(tag);

    return src_ingredients.each([&](const LanguageTypePair & key, const auto & ingredient)
    {
        MSS_BEGIN(bool);

        if (ingredient.propagation() == Propagation::Public && selection(key))
            MSS(dst_ingredients.insert_or_merge(key, ingredient));

        MSS_END();
    });

    MSS_END();
}

}

DependentPropagation::DependentPropagation()
    : selection_([](const LanguageTypePair & ) { return true; })
{

}

DependentPropagation::DependentPropagation(const SelectionFunction & function)
    : selection_(function)
{

}

Result DependentPropagation::process(const Context & context, model::Snapshot & snapshot, model::Snapshot & /*post*/) const
{
    MSS_BEGIN(Result);

    MSS(!!selection_);

    for (const model::Recipe * recipe: context.dependencies)
    {
        MSS( merge_(recipe->pre(), snapshot, selection_, model::tag::File_t() ) );
        MSS( merge_(recipe->pre(), snapshot, selection_, model::tag::KeyValue_t() ) );
    }


    MSS_END();
}

} }
