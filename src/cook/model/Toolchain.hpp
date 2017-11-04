#ifndef HEADER_cook_model_Toolchain_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Toolchain_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include "gubg/std/filesystem.hpp"
#include <string>

namespace cook { namespace model { 

    class Toolchain
    {
    public:
        struct CompileInfo
        {
            std::string cmd_c;
            std::string cmd_cpp;
            std::string cmd_asm;
            std::string flags;
            std::string defines;
        };
        struct LinkInfo
        {
            std::string cmd;
            std::string flags;
        };
        struct ArchiveInfo
        {
            std::string cmd;
            std::string flags;
        };

        void add_highlights(std::filesystem::path &path) const
        {
            path /= std::filesystem::path(arch_);
            path /= std::filesystem::path(config_);
        }

        void set_config(const std::string &config) { config_ = config; }
        void set_arch(const std::string &arch) { arch_ = arch; }

        bool get_info(CompileInfo &compile, LinkInfo &link, ArchiveInfo &archive) const
        {
            MSS_BEGIN(bool);
            if (false) {}
            else if (arch_ == "uno")
            {
                compile.cmd_c = "avr-gcc";
                compile.cmd_cpp = "avr-g++ -std=c++17";
                compile.cmd_asm = "avr-as";
                compile.flags += " -flto";
                compile.defines += " -DARDUINO=10610";
                compile.defines += " -DARDUINO_ARCH_AVR";
                compile.defines += " -DARDUINO_AVR_UNO";
                compile.defines += " -DF_CPU=16000000L";

                link.cmd = "avr-g++ -std=c++17";
                link.flags += " -flto";

                archive.cmd = "avr-gcc-ar";
                archive.flags = "rcs";
            }
            else 
            {
                compile.cmd_c = "gcc";
                compile.cmd_cpp = "g++ -std=c++17";
                compile.cmd_asm = "as";
                if (false) {}
                else if (config_ == "release") { compile.defines += " -DNDEBUG"; }
                else if (config_ == "debug") { compile.flags += " -g"; }

                link.cmd = "g++ -std=c++17";

                if (false) {}
                else if (arch_ == "x32")
                {
                    compile.flags += " -m32";
                    link.flags    += " -m32";
                }
                else if (arch_ == "x64")
                {
                    compile.flags += " -m64";
                    link.flags    += " -m64";
                }

                archive.cmd = "ar";
                archive.flags = "rcs";
            }
            MSS_END();
        }

    private:
        std::string arch_;
        std::string config_;
    };

} } 

#endif
