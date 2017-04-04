#ifndef HEADER_cook_Options_hpp_ALREADY_INCLUDED
#define HEADER_cook_Options_hpp_ALREADY_INCLUDED

#include "gubg/OptionParser.hpp"
#include "gubg/mss.hpp"

namespace cook { 
    struct Options
    {
        std::string config = "release";
        bool print_help = false;
        std::string help_message;
        bool do_build = true;
        std::string alias;

        bool parse(int argc, const char **argv)
        {
            MSS_BEGIN(bool);
            gubg::OptionParser opt("Cooking fresh binaries, C++-style");
            opt.add_switch('h', "--help", "Print this help", [&](){print_help = true;});
            opt.add_mandatory('c', "--config", "Configuration: [release|debug]", [&](const std::string &str){config = str;});
            opt.add_switch('n', "--no-build", "Only generate the build.ninja file", [&](){do_build = false;});
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
