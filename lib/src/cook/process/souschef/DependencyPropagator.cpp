#include "cook/process/souschef/DependencyPropagator.hpp"

namespace cook { namespace process { namespace souschef {

namespace  {

template <typename Tag>
Result merge_(const model::Recipe & src_recipe, model::Recipe & dst_recipe, const DependentPropagator::SelectionFunction & selection, Tag tag)
{
    MSS_BEGIN(Result);

    const auto & src_ingredients = src_recipe.ingredients(tag);
    auto & dst_ingredients = dst_recipe.ingredients(tag);

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

DependentPropagator::DependentPropagator()
    : selection_([](const LanguageTypePair & ) { return true; })
{

}

DependentPropagator::DependentPropagator(const SelectionFunction & function)
    : selection_(function)
{

}

Result DependentPropagator::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & /*context*/) const
{
    MSS_BEGIN(Result);

    MSS(!!selection_);

    for (const model::Recipe * src_recipe: recipe.dependencies())
    {
        MSS( merge_(*src_recipe, recipe, selection_, model::tag::File_t() ) );
        MSS( merge_(*src_recipe, recipe, selection_, model::tag::KeyValue_t() ) );
    }


    MSS_END();
}

} } }
