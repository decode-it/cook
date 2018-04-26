#ifndef HEADER_cook_process_command_Toolchain_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Toolchain_hpp_ALREADY_INCLUDED

#include "cook/process/command/Compiler.hpp"
#include "cook/Language.hpp"
#include "gubg/mss.hpp"
#include <string>

namespace cook { namespace process { namespace command { 

    class Toolchain
    {
    public:
        bool set_brand(const std::string &brand);

        bool initialize();

         Result create_compiler(CompilerPtr &, Language) const;

    private:
        std::string brand_;
    };

} } } 

#endif
