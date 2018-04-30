#ifndef HEADER_cook_app_Options_hpp_ALREADY_INCLUDED
#define HEADER_cook_app_Options_hpp_ALREADY_INCLUDED

#include "cook/log/Scope.hpp"
#include <string>
#include <list>
#include <ostream>

namespace cook { namespace app {

    struct Options
    {
        std::list<std::string> recipe_files;
        std::string output_path = "./";
        std::string temp_path = ".cook";
        std::string toolchain;
        std::string chef;
        std::list<std::pair<std::string, std::string>> generators;
        bool clean = false;
        std::list<std::pair<std::string, std::string>> variables;

        bool print_help = false;
        unsigned int verbosity = 1;

        std::list<std::string> recipes;

        std::string help_message;

        bool parse(int argc, const char **argv);
        bool valid() const;
        void stream(log::Importance importance = log::Importance{}) const;

    private:
        bool parsed_ = false;
    };

} }

#endif
