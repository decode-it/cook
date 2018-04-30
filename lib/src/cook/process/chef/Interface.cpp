#include "cook/process/chef/Interface.hpp"
#include "cook/log/Scope.hpp"

using namespace cook::model;

namespace cook { namespace process { namespace chef {

Result Interface::mis_en_place(Context & context)
{
    MSS_BEGIN(Result);

    auto s = log::scope("mis_en_place");

    Menu & menu = context.menu();

    MSS(menu.is_valid());

    const std::list<Recipe *> & order = menu.topological_order_recipes();

    // process every recipe in topological order
    for(model::Recipe * recipe : order)
    {
        auto ss = log::scope("recipe", [&](auto & n) {n.attr(recipe->uri().string()); });

        // get the brigade for this recipe
        const Brigade * brigade = nullptr;
        MSS(find_brigade(brigade, recipe));
        MSS(!!brigade);

        // get the graph
        RecipeFilteredGraph * graph = menu.recipe_filtered_graph(recipe);
        MSS(!!graph);

        MSS(mis_en_place_(*recipe, *graph, context, *brigade));

        recipe->stream();
    }

    MSS_END();
}

Result Interface::mis_en_place_(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context &context, const Brigade & brigade) const
{
    MSS_BEGIN(Result);


    for(SouschefPtr souschef : brigade.souschefs)
    {
        auto ss = log::scope("souschef", [&](auto & n) {n.attr("description", souschef->description()); });
        MSS(souschef->process(recipe, file_command_graph, context));
    }

    MSS_END();
}

Result Interface::find_brigade(const Brigade *& brigade, model::Recipe * recipe) const
{
    MSS_BEGIN(Result);

    auto s = log::scope("find brigade");

    for(auto it = brigade_priority_map_.begin(); it != brigade_priority_map_.end(); ++it)
    {
        const Brigade & candidate = it->second;

        std::string reason;
        if (!candidate.can_cook(recipe, reason))
        {
            MSS_RC << MESSAGE(Info, "Skipping instruction set '" << candidate.name << "': " << reason);
        }
        else
        {
            brigade = &candidate;
            MSS_RETURN_OK();
        }
    }

    MSG_MSS(false, Error, "No way found to build recipe '" << recipe->uri() << "'");
    MSS_END();
}

} } }


