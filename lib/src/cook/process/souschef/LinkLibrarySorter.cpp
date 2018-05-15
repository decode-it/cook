#include "cook/process/souschef/LinkLibrarySorter.hpp"

namespace cook { namespace process { namespace souschef {

using LibType = ingredient::KeyValue;

Result LinkLibrarySorter::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & context) const
{
    MSS_BEGIN(Result);

    auto it = recipe.key_values().find(LanguageTypePair(Language::Binary, Type::Library));
    if (it == recipe.key_values().end())
        MSS_RETURN_OK();

    MSS(process_(recipe, it->second, context.menu()));

    MSS_END();
}

Result LinkLibrarySorter::process_(model::Recipe & recipe, ingredient::Collection<LibType> & libraries, const process::Menu & menu) const
{
    MSS_BEGIN(Result);

    std::list<LibType> user_supplied_libs;
    std::unordered_map<model::Recipe *, std::list<LibType>> generated_libs;

    // store all the libraries, either as non-owned or as owned
    for(const LibType & file : libraries)
        if (!is_internal_generated(file.content()))
            user_supplied_libs.push_back(file);
        else
            generated_libs[file.owner()].push_back(file);

    const unsigned int prev_size = libraries.size();
    libraries.clear();

    // get a topological order for this recipe
    std::list<model::Recipe *> top_order;
    MSS(menu.topological_order_recipes(&recipe, top_order));

    // add the owned in topological inverse order
    for (auto it = top_order.rbegin(); it != top_order.rend(); ++it)
    {
        model::Recipe * cur = *it;
        auto it2 = generated_libs.find(cur);
        if (it2 != generated_libs.end())
            for(const auto & file : it2->second)
                libraries.insert(file);
    }

    // add the non-owned
    for(const auto & file : user_supplied_libs)
        libraries.insert(file);

    MSS(prev_size == libraries.size());

    MSS_END();
}


} } }

