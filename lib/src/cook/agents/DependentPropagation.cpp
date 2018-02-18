#include "cook/agents/DependentPropagation.hpp"

namespace cook { namespace agents {

namespace  {

template <typename PropertyExtractor>
bool process_(PropertyExtractor && propertyExtractor, const DependentPropagation::SelectionFunction & selection, const Context & context, model::Snapshot & snapshot)
{
    MSS_BEGIN(bool);

    auto & properties = propertyExtractor(snapshot);
    auto merger = [&](const LanguageTypePair & key, const auto & property)
    {
        MSS_BEGIN(bool);

        if (property.propagation() == Propagation::Public)
            if (selection && selection(key))
                MSS(properties.insert_or_merge(key, property));

        MSS_END();
    };

    // merge all files
    for(model::Recipe * recipe : context.dependencies)
        MSS(propertyExtractor(*recipe).each(merger));

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
        auto file_extractor = [](model::Snapshot & snapshot) -> property::Properties<property::File> & { return snapshot.file_properties(); };
        MSS(process_(file_extractor, selection_, context, snapshot));
    }

    {
        auto key_value_extractor = [](model::Snapshot & snapshot) -> property::Properties<property::KeyValue> & { return snapshot.key_value_properties(); };
        MSS(process_(key_value_extractor, selection_, context, snapshot));
    }

    MSS_END();
}

} }
