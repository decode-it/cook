#include "cook/chef/DependentPropagation.hpp"

namespace cook { namespace chef {

namespace  {

template <typename IngredientExtractor>
bool process_(IngredientExtractor && ingredient_extractor, const DependentPropagation::SelectionFunction & selection, const Context & context, model::Snapshot & snapshot)
{
    MSS_BEGIN(bool);

    auto & properties = ingredient_extractor(snapshot);
    auto merger = [&](const LanguageTypePair & key, const auto & ingredient)
    {
        MSS_BEGIN(bool);

        if (ingredient.propagation() == Propagation::Public)
            if (selection && selection(key))
                MSS(properties.insert_or_merge(key, ingredient));

        MSS_END();
    };

    // merge all files
    for(model::Recipe * recipe : context.dependencies)
        MSS(ingredient_extractor(*recipe).each(merger));

    MSS_END();
}

}

DependentPropagation::DependentPropagation(const SelectionFunction & function)
    : selection_(function)
{

}

bool DependentPropagation::process(const Context & context, model::Snapshot & snapshot, model::Snapshot & /*post*/) const
{
    MSS_BEGIN(bool);

    {
        auto file_extractor = [](model::Snapshot & snapshot) -> ingredient::Ingredients<ingredient::File> & { return snapshot.files(); };
        MSS(process_(file_extractor, selection_, context, snapshot));
    }

    {
        auto key_value_extractor = [](model::Snapshot & snapshot) -> ingredient::Ingredients<ingredient::KeyValue> & { return snapshot.key_values(); };
        MSS(process_(key_value_extractor, selection_, context, snapshot));
    }

    MSS_END();
}

} }
