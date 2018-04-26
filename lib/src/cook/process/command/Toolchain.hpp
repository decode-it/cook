#ifndef HEADER_cook_process_command_Toolchain_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Toolchain_hpp_ALREADY_INCLUDED

#include "cook/process/command/Compiler.hpp"
#include "cook/process/command/Archiver.hpp"
#include "cook/process/command/Linker.hpp"
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
         Result create_archiver(ArchiverPtr &) const;
         Result create_linker(LinkerPtr &) const;

    private:
        std::string brand_;
    };

} } } 

#endif
