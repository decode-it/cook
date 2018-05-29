#include "cook/process/souschef/DependencyPropagator.hpp"
#include "cook/util/File.hpp"

namespace cook { namespace process { namespace souschef {

namespace  {


template <typename Tag, typename Functor, typename Transformer>
Result merge_(const model::Recipe & src_recipe, model::Recipe & dst_recipe, const DependentPropagator::SelectionFunction & selection, Functor && functor, Transformer && transform, Tag tag)
{
    MSS_BEGIN(Result);

    const auto & src_ingredients = src_recipe.ingredients(tag);
    auto & dst_ingredients = dst_recipe.ingredients(tag);

    return src_ingredients.each([&](LanguageTypePair key, auto ingredient)
    {
        MSS_BEGIN(Result);

        if (ingredient.propagation() == Propagation::Public && selection(key))
        {
            if (!functor || functor(key, ingredient))
                MSS(dst_ingredients.insert_or_merge(key, transform(ingredient)));
        }

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

    for (const model::Recipe::Dependencies::value_type & p: recipe.dependency_pairs())
    {
        const model::Recipe * src_recipe = p.second.recipe;

        auto ss = log::scope("Merging recipes", [&](auto & n)
        {
            n.attr("src", src_recipe->uri()).attr("tgt", recipe.uri());
        });

        {
            const std::filesystem::path & tr = util::get_from_to_path(recipe, *src_recipe);
            auto transform = [&](const ingredient::File & f) 
            { 
                return util::combine_file(tr, f);
            };
            MSS( merge_(*src_recipe, recipe, selection_, p.second.file_filter, transform, model::tag::File_t() ) );
        }
        {
            auto transform = [&](const auto & f) { return f; };
            MSS( merge_(*src_recipe, recipe, selection_, p.second.key_value_filter, transform, model::tag::KeyValue_t() ) );
        }
    }


    MSS_END();
}

} } }
