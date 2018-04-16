#include "cook/process/souschef/LinkLibrarySorter.hpp"

namespace cook { namespace process { namespace souschef {

Result LinkLibrarySorter::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & context) const
{
    MSS_BEGIN(Result);

    model::Recipe::Files & files = recipe.files();

    auto it = files.find(LanguageTypePair(Language::Binary, Type::Library));
    if (it == files.end())
        MSS_RETURN_OK();

    MSS(process_(recipe, it->second, context.menu()));

    MSS_END();
}

Result LinkLibrarySorter::process_(model::Recipe & recipe, ingredient::Collection<ingredient::File> & libraries, const process::Menu & menu) const
{
    MSS_BEGIN(Result);

    std::list<ingredient::File> non_owned_libraries;
    std::unordered_map<model::Recipe *, std::list<ingredient::File>> owned_libraries;

    // store all the libraries, either as non-owned or as owned
    for(const auto & file : libraries)
        if (file.owner() == nullptr)
            non_owned_libraries.push_back(file);
        else
            owned_libraries[file.owner()].push_back(file);

    const unsigned int prev_size = libraries.size();
    libraries.clear();

    // get a topological order for this recipe
    std::list<model::Recipe *> top_order;
    MSS(menu.topological_order_recipes(&recipe, top_order));

    // add the owned in topological inverse order
    for (auto it = top_order.rbegin(); it != top_order.rend(); ++it)
    {
        model::Recipe * cur = *it;
        auto it2 = owned_libraries.find(cur);
        if (it2 != owned_libraries.end())
            for(const auto & file : it2->second)
                libraries.insert(file);
    }

    // add the non-owned
    for(const auto & file : non_owned_libraries)
        libraries.insert(file);

    MSS(prev_size == libraries.size());

    MSS_END();
}


} } }

