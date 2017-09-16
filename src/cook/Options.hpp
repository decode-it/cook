#ifndef HEADER_cook_Options_hpp_ALREADY_INCLUDED
#define HEADER_cook_Options_hpp_ALREADY_INCLUDED

#include "gubg/OptionParser.hpp"
#include "gubg/mss.hpp"

namespace cook { 
    struct Options
    {
        enum Mode
        {
            Unknown = 0x00,
            New = 0x01,
            Existing = 0x02,
            Help = 0x04
        };
        
        
        std::string config = "release";
        std::string arch = "x32";
        bool print_help = false;
        std::string help_message;
        unsigned int verbose = 0;
        bool clean = false;
        bool do_build = true;
        bool print_recipes = false;
        std::string uri;
        std::string project_name;
        std::string build_dir = ".cook";
        std::string source_dir = "";
        std::string deploy_dir = "";

        bool parse(int argc, const char **argv)
        {
            MSS_BEGIN(bool);
            gubg::OptionParser opt("Cooking fresh binaries, C++-style");
            
            opt.add_switch('v', "--verbose", "Verbose output",                                                      [&](){verbose = 1;});
            opt.add_switch('V', "--very-verbose", "Very verbose output",                                            [&](){verbose = 2;});
            opt.add_mandatory('s', "--source-dir", "Specify the source directory",                                  [&](std::string str){source_dir = str; });
            opt.add_switch('h', "--help", "Print this help. Runs cook in Help mode.",                               [&](){print_help = true;                        mode_ |= Help; });
            opt.add_switch('C', "--clean", "Clean-up temporary build results. Runs cook in Existing mode.",         [&](){clean = true;                             mode_ |= Existing; });
            opt.add_mandatory('c', "--config", "Configuration: [release|debug]. Runs cook in Existing mode.",       [&](std::string str){config = str;              mode_ |= Existing; });
            opt.add_mandatory('a', "--arch", "Architecture: [x32|x64|uno]. Runs cook in Existing mode.",            [&](std::string str){arch = str;                mode_ |= Existing; });
            opt.add_switch('n', "--no-build", "Only generate the build.ninja file. Runs cook in Existing mode.",    [&](){do_build = false;                         mode_ |= Existing; });
            opt.add_switch('t', "--tree", "Print all recipes in tree format. Runs cook in Existing mode.",          [&](){print_recipes = true;                     mode_ |= Existing; });
            opt.add_mandatory('b', "--build-dir", "Specify the build directory. Runs cook in Existing mode.",       [&](std::string str){build_dir = str;           mode_ |= Existing; });
            opt.add_mandatory('d', "--deploy-dir", "Specify the deployment directory. Runs cook in Existing mode.", [&](std::string str){deploy_dir = str;          mode_ |= Existing; });
            opt.add_mandatory('p', "--project", "Create new project with <name>.Runs cook in New mode.",            [&](std::string str){project_name = str;        mode_ |= New; });
            
            auto args = gubg::OptionParser::create_args(argc, argv);
            MSS(opt.parse(args));           
            
            {
                std::ostringstream str;
                
                {
                    Mode m;
                    if (!get_mode(m))
                        str << "Cook called in multiple modes. I've got no idea what to do!" << std::endl;
                }
                
                str << opt.help();
                str << "cook version 1.0.0";
                help_message = str.str();
            }
            
            if (args.size() == 1)
                uri = args.front();
            
            MSS_END();
        }
        
        bool get_mode(Mode & mode) const
        {
            MSS_BEGIN(bool);
            if (mode_ == Unknown)
                mode = Existing;
            else if ((mode_ & (mode_-1)) == 0)
                mode = static_cast<Mode>(mode_);
            else
                MSS(false);
            MSS_END();
        }
        
    private:
        unsigned int mode_ = Unknown;
    };
} 

#endif
