#include "cook/process/souschef/ScriptRunner.hpp"
#include "cook/Result.hpp"
#include "boost/process.hpp"

namespace cook { namespace process { namespace souschef {

Result ScriptRunner::process(model::Recipe & recipe, RecipeFilteredGraph & /*file_command_graph*/, const Context & context) const
{
    MSS_BEGIN(Result);
    MSS(recipe.build_target().type == TargetType::Script);

    std::list<ingredient::File> script_files;

    auto gather_scripts = [&](const LanguageTypePair & ltp, ingredient::File & file)
    {
        MSS_BEGIN(Result);

        if(ltp.type != Type::Script)
            MSS_RETURN_OK();

        MSG_MSS(script_files.empty(), Error, "Only a single script is allowed");
        script_files.push_back(file);

        file.set_propagation(Propagation::Private);
        MSS_END();
    };

    MSS(recipe.files().each(gather_scripts));
    MSG_MSS(!script_files.empty(), Error, "A single script should be given");

    std::list<std::string> arguments;
    recipe.key_values().each([&](const LanguageTypePair & ltp, ingredient::KeyValue & key_value)
    {
        MSS_BEGIN(Result);
        if (ltp.type != Type::ScriptArgument)
            MSS_RETURN_OK();

        arguments.push_back(key_value.key());
        MSS_END();
    });

    const ingredient::File & script = script_files.front();
    int retval = boost::process::system(script.key().string(), boost::process::args = arguments);

    MSG_MSS(retval == 0, Error, "Executing of script " << script << " failed with code " << retval);

    MSS_END();
}

} } }
