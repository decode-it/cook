#include "cook/process/souschef/ScriptRunner.hpp"
#include "cook/Result.hpp"
#include "boost/process.hpp"

namespace cook { namespace process { namespace souschef {

Result ScriptRunner::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & context) const
{
    MSS_BEGIN(Result, "");

    MSS(recipe.build_target().type == TargetType::Script);

#if 0
    std::list<ingredient::File> script_files;

    for(auto & file : recipe.files().range(LanguageTypePair(Language::Script, Type::Executable)))
    {
        MSG_MSS(script_files.empty(), Error, "Only a single script is allowed");
        script_files.push_back(file);

        file.set_propagation(Propagation::Private);
    };
    MSG_MSS(!script_files.empty(), Error, "A single script should be given");


    std::list<std::string> arguments;
    for(auto & arg : recipe.key_values().range(LanguageTypePair(Language::Script, Type::Argument)))
    {
        arguments.push_back(arg.key());
        arg.set_propagation(Propagation::Private);
    }

    const ingredient::File & script = script_files.front();
    int retval = boost::process::system(script.key().string(), boost::process::args = arguments);

    MSG_MSS(retval == 0, Error, "Executing of script " << script << " failed with code " << retval);
#else
    for(auto & cmd : recipe.key_values().range(LanguageTypePair(Language::Script, Type::Executable)))
    {
        cmd.set_propagation(Propagation::Private);

        const auto command = cmd.key();
        const int retval = boost::process::system(command);
        MSG_MSS(retval == 0, Error, "Executing of script " << command << " failed with code " << retval);
    }
#endif

    MSS_END();
}

} } }
