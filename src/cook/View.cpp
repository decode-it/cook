#include "cook/View.hpp"
#include "cook/view/Options.hpp"
#include "cook/view/chai/Runner.hpp"
#include "gubg/mss.hpp"

namespace cook { 

    bool View::process_cli(int argc, const char **argv)
    {
        MSS_BEGIN(bool);

        view::Options options;
        MSS(options.parse(argc, argv));

        MSS(presenter_.set("help.message", options.help_message));
        MSS(presenter_.set("env.build_dir", options.build_dir));
        MSS(presenter_.set("toolchain.config", options.config));
        MSS(presenter_.set("toolchain.arch", options.arch));
        if (options.print_help)
            MSS(presenter_.set("action.print.help"));

        view::chai::Runner chai_runner(presenter_, logger_);
        MSS(chai_runner.execute(options.input_fod));

        MSS(presenter_.set("action.print.books"));

        if (false) {}
        else if (options.generate == "build.ninja")
            MSS(presenter_.set("action.generate.ninja", options.uri));
        else if (options.generate == "details.tree")
            MSS(presenter_.set("action.generate.details", options.uri));
        else if (options.generate == "structure.tree" || options.generate == "recipes.tree")
            MSS(presenter_.set("action.generate.structure", options.uri));

        MSS_END();
    }

} 
