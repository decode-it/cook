#include "cook/View.hpp"
#include "cook/view/Options.hpp"
#include "cook/view/chai/Runner.hpp"
#include "cook/Presenter.hpp"
#include "gubg/mss.hpp"

namespace cook { 

    using C = presenter::Command;

    bool View::process_cli(int argc, const char **argv)
    {
        MSS_BEGIN(bool);

        view::Options options;
        MSS(options.parse(argc, argv));

        logger_.set_verbose(options.verbose);

        log() << "Cook is starting" << std::endl;

        MSS(presenter_->set( {C::help, C::message },           as_any(options.help_message) ));
        MSS(presenter_->set( {C::env, C::dir, C::temp },       as_any(options.temp_build_dir) ));
        MSS(presenter_->set( {C::env, C::dir, C::output },     as_any(options.output_dir) ));
        MSS(presenter_->set( {C::toolchain, C::name},          as_any(options.toolchain) ));
        MSS(presenter_->set( {C::toolchain, C::config},        as_any(options.config) ));
        MSS(presenter_->set( {C::toolchain, C::arch},          as_any(options.arch) ));
        if (options.print_help)
        {
            MSS(presenter_->set( {C::action, C::print, C::help} ));
            MSS_RETURN_OK();
        }

        view::chai::Runner chai_runner(presenter_, logger_);
        MSS(chai_runner.process(options.input_files));

        MSS(presenter_->set( {C::runner, C::done} ));

        if (options.verbose >= 2)
            MSS(presenter_->set( {C::action, C::print, C::books} ));

        if (false) {}
        else if (options.generate == "build.ninja")
            MSS(presenter_->set( {C::action, C::generate, C::ninja}, as_any(options.uri) ));
        else if (options.generate == "details.naft")
            MSS(presenter_->set( {C::action, C::generate, C::details}, as_any(options.uri) ));
        else if (options.generate == "structure.naft")
            MSS(presenter_->set( {C::action, C::generate, C::structure} ));

        log() << "Cook is stopping" << std::endl;

        MSS_END();
    }

} 
