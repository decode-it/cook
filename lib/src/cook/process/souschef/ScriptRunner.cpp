#include "cook/process/souschef/ScriptRunner.hpp"
#include "cook/Result.hpp"
#include "boost/process.hpp"

namespace cook { namespace process { namespace souschef {

Result ScriptRunner::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & context) const
{
    MSS_BEGIN(Result, "");

    MSS(recipe.build_target().type == TargetType::Script);

    for(auto & cmd : recipe.key_values().range(LanguageTypePair(Language::Script, Type::Executable)))
    {
        cmd.set_propagation(Propagation::Private);

        const auto command = cmd.key();
        const int retval = boost::process::system(command);
        MSG_MSS(retval == 0, Error, "Executing of script " << command << " failed with code " << retval);
    }

    MSS_END();
}

} } }
