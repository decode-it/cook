#include "cook/api/GeneratorManager.h"
#include "cook/View.hpp"
#include "cook/view/Options.hpp"
#include "cook/view/chai/Runner.hpp"
#include "gubg/mss.hpp"

#include "cook/API.h"

namespace cook { 

bool View::process_cli(cook_Handle_t api, int argc, const char **argv)
{
    MSS_BEGIN(bool);

    view::Options options;
    MSS(options.parse(argc, argv));

    std::list<std::string> input_files = options.input_files;
    if (input_files.empty())
        input_files.push_back("");

    logger_.set_verbose(options.verbose);

    log() << "Cook is starting" << std::endl;


    cook_API_set_cache_dir(api, options.temp_build_dir.c_str());
    cook_API_set_output_dir(api, options.output_dir.c_str());
    cook_API_set_toolchain_name(api, options.toolchain.c_str());
    cook_API_set_toolchain_config(api, options.config.c_str());
    cook_API_set_toolchain_arch(api, options.arch.c_str());

    if (options.print_help)
    {
        std::cout << options.help_message << std::endl;
        MSS_RETURN_OK();
    }

    view::chai::Runner chai_runner(api, logger_);
    MSS(chai_runner.process(input_files));

    MSS(cook_API_resolve(api) == COOK_TRUE);


    {
        cook_api_Element_t mgr = cook_API_generator_manager(api);
        cook_api_GenerationType_t generation = {};

        if (false) {}
        else if (options.generate == "build.ninja")
            generation = cook_api_GenerationType_Ninja;
        else if (options.generate == "structure.naft")
            generation = cook_api_GenerationType_Naft;

        if (options.uri.empty())
            MSS(cook_api_GeneratorManager_generate(mgr, generation, 0, nullptr) == COOK_TRUE);
        else
        {
            const char * argv[] = { options.uri.c_str() };
            MSS(cook_api_GeneratorManager_generate(mgr, generation, 1, argv) == COOK_TRUE);
        }

    }


    log() << "Cook is stopping" << std::endl;

    MSS_END();
}

} 
