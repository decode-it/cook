#include "cook/process/souschef/LinkLibrarySorter.hpp"

namespace cook { namespace process { namespace souschef {

using LibType = ingredient::KeyValue;

Result LinkLibrarySorter::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & context) const
{
    MSS_BEGIN(Result);

    std::list<LibType> user_supplied_libs;
    std::unordered_map<model::Recipe *, std::list<LibType>> generated_libs;

    const auto ltp = LanguageTypePair(Language::Binary, Type::Library);

    // store all the libraries, either as non-owned or as owned
    recipe.each_key_value(ltp, [&](const LibType &kv) {
        if (!is_internal_generated(kv.content()))
            user_supplied_libs.push_back(kv);
        else
            generated_libs[kv.owner()].push_back(kv);

        return true;
    });

    // no generator libs, nothing to do
    if (generated_libs.empty())
        MSS_RETURN_OK();

    // erase all the libraries
    recipe.erase(ltp, model::tag::KeyValue_t());
    
    // get a topological order for this recipe
    std::list<model::Recipe *> top_order;
    MSS(context.menu().topological_order_recipes(&recipe, top_order));

    // add the owned in topological inverse order
    for (auto it = top_order.rbegin(); it != top_order.rend(); ++it)
    {
        model::Recipe * cur = *it;
        auto it2 = generated_libs.find(cur);
        if (it2 != generated_libs.end())
            for(const auto & kv : it2->second)
                MSS(recipe.insert(ltp, kv));
    }

    // add the non-owned
    for(const auto & kv : user_supplied_libs)
        MSS(recipe.insert(ltp, kv));

    MSS_END();
}


} } }

