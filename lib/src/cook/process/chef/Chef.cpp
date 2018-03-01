#include "cook/process/chef/Chef.hpp"

using namespace cook::model;

namespace cook { namespace process { namespace chef {

Result Chef::mis_en_place(Context & context)
{
    MSS_BEGIN(Result);

    const auto & menu = context.menu();

    MSS(menu.is_valid());

    const std::list<Recipe *> & order = menu.topological_order();

    // process every element in inverse topological order
    for(auto it = order.rbegin(); it != order.rend(); ++it)
    {
        Recipe * recipe = *it;

        const InstructionSet * instruction_set = nullptr;
        MSS(find_instruction_set(instruction_set, recipe));

        souschef::Context sc;
        MSS(prepare_context(sc, recipe, menu, &context.dirs()));


        MSS(!!instruction_set);
        MSS(mis_en_place_(context, sc, *instruction_set));
    }


    MSS_END();
}

Result Chef::mis_en_place_(Context &kitchen, const souschef::Context & context, const InstructionSet & instruction_set) const
{
    MSS_BEGIN(Result);

    const std::string & name = context.recipe->uri().string();

    for(AssistantPtr assistant : instruction_set.assistants)
    {
        kitchen.logger().LOG(Info, "[" << name << "]: " << assistant->description());
        MSS(assistant->process(context, context.recipe->pre()));
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

Result Chef::prepare_context(souschef::Context & context, model::Recipe * recipe, const Menu & menu, const Dirs * dirs)
{
    MSS_BEGIN(Result);

    MSS(!!dirs);
    context.dirs = dirs;

    MSS(!!recipe);
    context.recipe = recipe;

    for(Recipe * dep : recipe->dependencies())
    {
        MSS(!!dep);
        context.dependencies.push_back(dep);
    }

//    MSS(menu.topological_order(recipe, context.topological_order));

    MSS_END();
}




} } }


