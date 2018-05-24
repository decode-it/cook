#include "cook/process/souschef/RecipeNamer.hpp"
#include "cook/process/toolchain/Manager.hpp"

namespace cook { namespace process { namespace souschef {

Result RecipeNamer::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & context) const
{
    MSS_BEGIN(Result);

    auto output = context.toolchain().get_primary_target(recipe);

    if (output.empty())
        recipe.build_target().filename.reset();
    else
        recipe.build_target().filename = output;

    MSS_END();
}

} } }

