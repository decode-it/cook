#include "cook/agents/LinkOrder.hpp"

namespace cook { namespace agents {

LinkOrder::LinkOrder(const SelectionFunction & selection_function)
    : selection_(selection_function)
{

}

bool LinkOrder::process(const Context & context, model::Snapshot & snapshot, model::Snapshot & post) const
{
    MSS_BEGIN(bool);

    for ( auto & p : snapshot.file_properties())
        if (p.first.type == Type::Library)
            if (selection_ && selection_(p.first.language))
                MSS(process_(context, snapshot, p.second));

    MSS_END();
}

bool LinkOrder::process_(const Context & context, model::Snapshot & snapshot, property::Collection<property::File> & libraries) const
{
    MSS_BEGIN(bool);

    std::list<property::File> non_owned_libraries;
    std::map<model::Recipe *, std::list<property::File>> owned_libraries;

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


} }

