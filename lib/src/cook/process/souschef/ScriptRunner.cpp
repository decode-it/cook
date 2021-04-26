#include "cook/process/souschef/ScriptRunner.hpp"
#include <cstdlib>
#include "cook/Result.hpp"

namespace cook { namespace process { namespace souschef {

    Result ScriptRunner::process(model::Recipe &recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context &context) const
    {
        MSS_BEGIN(Result, "");

        MSS(recipe.build_target().type == TargetType::Script);

        MSS(recipe.each_key_value(LanguageTypePair(Language::Script, Type::Executable), [=](auto &cmd) {
            MSS_BEGIN(Result, "");
            cmd.set_propagation(Propagation::Private);

            if (do_execute_)
            {
                const auto command = cmd.key();
                const int retval = std::system(command.c_str());
                MSG_MSS(retval == 0, Error, "Executing of script " << command << " failed with code " << retval);
            }
            MSS_END();
        }));

        MSS_END();
    }

}}}
