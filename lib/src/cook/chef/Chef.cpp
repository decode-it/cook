#include "cook/chef/Chef.hpp"

using namespace cook::model;

namespace cook { namespace chef {

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

        assistant::Context context;
        MSS(prepare_context(context, recipe, menu, kitchen.environment()));


        MSS(!!instruction_set);
        MSS(mis_en_place_(kitchen, context, instruction_set->instructions));
    }


    MSS_END();
}

Result Chef::mis_en_place_(Kitchen & kitchen, const assistant::Context & context, const Instructions & instructions) const
{
    MSS_BEGIN(Result);

    kitchen.logger().LOG(Info, "Preparing " << context.recipe->uri());

    for(const Instruction & instruction : instructions)
    {
        kitchen.logger().LOG(Info, instruction.description);

        for (Instruction::UtensilPtr tool : instruction.steps)
            MSS(tool->process(context, context.recipe->pre()));
    }

    MSS_END();
}

Result Chef::find_instruction_set(const InstructionSet *& result, model::Recipe * recipe) const
{
    MSS_BEGIN(Result);

    for(auto it = instruction_set_priority_map_.begin(); it != instruction_set_priority_map_.end(); ++it)
    {
        const InstructionSet & candidate = it->second;
        if (candidate.decision_function(recipe))
        {
            result = &candidate;
            MSS_RETURN_OK();
        }
    }

    MSG_MSS(false, Error, "No way found to build recipe '" << recipe->uri() << "'");
    MSS_END();
}

Result Chef::prepare_context(assistant::Context & context, model::Recipe * recipe, const model::Menu & menu, model::Environment * environment)
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




} }


