#include "cook/process/chef/Chef.hpp"

using namespace cook::model;

namespace cook { namespace process { namespace chef {

Result Chef::mis_en_place(Context & context)
{
    MSS_BEGIN(Result);

    Menu & menu = context.menu();

    MSS(menu.is_valid());

    const std::list<Recipe *> & order = menu.topological_order_recipes();

    // process every recipe in topological order
    for(model::Recipe * recipe : order)
    {
        // get the instruction set for this recipe
        const InstructionSet * instruction_set = nullptr;
        MSS(find_instruction_set(instruction_set, recipe));
        MSS(!!instruction_set);

        // get the graph
        RecipeFilteredGraph * graph = menu.recipe_filtered_graph(recipe);
        MSS(!!graph);

        MSS(mis_en_place_(*recipe, *graph, context, *instruction_set));
    }

    MSS_END();
}

Result Chef::mis_en_place_(model::Recipe & recipe, RecipeFilteredGraph & file_command_graph, const Context &context, const InstructionSet &instruction_set) const
{
    MSS_BEGIN(Result);

    const std::string & name = recipe.uri().string();

    for(AssistantPtr assistant : instruction_set.assistants)
    {
        context.logger().LOG(Info, "[" << name << "]: " << assistant->description());
        MSS(assistant->process(recipe, file_command_graph, context));
    }

    MSS_END();
}

Result Chef::find_instruction_set(const InstructionSet *& result, model::Recipe * recipe) const
{
    MSS_BEGIN(Result);

    for(auto it = instruction_set_priority_map_.begin(); it != instruction_set_priority_map_.end(); ++it)
    {
        const InstructionSet & candidate = it->second;

        std::string reason;
        if (!candidate.can_cook(recipe, reason))
        {
            MSS_RC << MESSAGE(Info, "Skipping instruction set '" << candidate.name << "': " << reason);
        }
        else
        {
            result = &candidate;
            MSS_RETURN_OK();
        }
    }

    MSG_MSS(false, Error, "No way found to build recipe '" << recipe->uri() << "'");
    MSS_END();
}

} } }


