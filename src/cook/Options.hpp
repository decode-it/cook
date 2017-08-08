#ifndef HEADER_cook_Options_hpp_ALREADY_INCLUDED
#define HEADER_cook_Options_hpp_ALREADY_INCLUDED

#include "gubg/OptionParser.hpp"
#include "gubg/mss.hpp"

namespace cook { 
    struct Options
    {
        std::string config = "release";
        std::string arch = "x32";
        bool print_help = false;
        std::string help_message;
        int verbose = 0;
        bool clean = false;
        bool do_build = true;
        std::string alias;
        std::string project_name;

        bool parse(int argc, const char **argv)
        {
            MSS_BEGIN(bool);
            gubg::OptionParser opt("Cooking fresh binaries, C++-style");
            opt.add_switch('h', "--help", "Print this help", [&](){print_help = true;});
            opt.add_switch('v', "--verbose", "Verbose output", [&](){verbose = 1;});
            opt.add_switch('V', "--very-verbose", "Very verbose output", [&](){verbose = 2;});
            opt.add_switch('C', "--clean", "Clean-up temporary build results", [&](){clean = true;});
            opt.add_mandatory('c', "--config", "Configuration: [release|debug]", [&](std::string str){config = str;});
            opt.add_mandatory('a', "--arch", "Architecture: [x32|x64|uno]", [&](std::string str){arch = str;});
            opt.add_switch('n', "--no-build", "Only generate the build.ninja file", [&](){do_build = false;});
            opt.add_mandatory('p', "--project", "Create new project with <name>", [&](std::string str){project_name = str;});
            auto args = gubg::OptionParser::create_args(argc, argv);
            MSS(opt.parse(args));
            help_message = opt.help();
            if (args.size() == 1)
                alias = args.front();
            MSS_END();
        }
    };
} 

#endif
