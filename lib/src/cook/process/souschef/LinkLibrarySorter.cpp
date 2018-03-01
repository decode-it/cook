#include "cook/process/souschef/LinkLibrarySorter.hpp"

namespace cook { namespace process { namespace souschef {

Result LinkLibrarySorter::process(const Context & context, model::Recipe & recipe) const
{
    MSS_BEGIN(Result);

    model::Recipe::Files & files = recipe.files();

    auto it = files.find(LanguageTypePair(Language::Binary, Type::Library));
    if (it == files.end())
        MSS_RETURN_OK();

    MSS(process_(context, recipe, it->second));

    MSS_END();
}

Result LinkLibrarySorter::process_(const Context & context, model::Recipe & recipe, ingredient::Collection<ingredient::File> & libraries) const
{
    MSS_BEGIN(Result);

    std::list<ingredient::File> non_owned_libraries;
    std::map<model::Recipe *, std::list<ingredient::File>> owned_libraries;

    // store all the libraries, either as non-owned or as owned
    for(const auto & file : libraries)
        if (file.owner() == nullptr)
            non_owned_libraries.push_back(file);
        else
            owned_libraries[file.owner()].push_back(file);

    const unsigned int prev_size = libraries.size();
    libraries.clear();

    // add the non-owned
    for(const auto & file : non_owned_libraries)
        libraries.insert(file);

    // add the owned in topological inverse order
    for (auto recipe_it = context.topological_order.rbegin(); recipe_it != context.topological_order.rend(); ++recipe_it)
    {
        auto it = owned_libraries.find(*recipe_it);
        for(const auto & file : it->second)
            libraries.insert(file);
    }

    MSS(prev_size == libraries.size());

    MSS_END();
}


} } }

