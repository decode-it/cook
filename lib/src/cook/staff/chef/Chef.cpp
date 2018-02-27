#include "cook/staff/chef/Chef.hpp"

using namespace cook::model;

namespace cook { namespace staff { namespace chef {

Result Chef::mis_en_place(Kitchen & kitchen, const Menu & menu)
{
    MSS_BEGIN(Result);

    MSG_MSS(menu.all_dependencies_resolved(), InternalError, "Menu has unresolved dependencies");
    MSG_MSS(menu.is_acyclic(), InternalError, "Menu has cycles");

    const std::list<Recipe *> & order = menu.topological_order();

    // process every element in inverse topological order
    for(auto it = order.rbegin(); it != order.rend(); ++it)
    {
        Recipe * recipe = *it;

        const InstructionSet * instruction_set = nullptr;
        MSS(find_instruction_set(instruction_set, recipe));

        souschef::Context context;
        MSS(prepare_context(context, recipe, menu, kitchen.environment()));


        MSS(!!instruction_set);
        MSS(mis_en_place_(kitchen, context, *instruction_set));
    }


    MSS_END();
}

Result Chef::mis_en_place_(Kitchen & kitchen, const souschef::Context & context, const InstructionSet & instruction_set) const
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

Result Chef::prepare_context(souschef::Context & context, model::Recipe * recipe, const model::Menu & menu, model::Environment * environment)
{
    MSS_BEGIN(Result);

    MSS(!!environment);
    context.environment = environment;

    MSS(!!recipe);
    context.recipe = recipe;

    for(Recipe * dep : recipe->dependencies())
    {
        MSS(!!dep);
        context.dependencies.push_back(dep);
    }

    MSS(menu.topological_order(recipe, context.topological_order));

    MSS_END();
}




} } }


