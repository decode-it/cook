#ifndef HEADER_cook_model_Toolchain_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Toolchain_hpp_ALREADY_INCLUDED

#include "cook/model/toolchain/GCC.hpp"
#include "cook/model/toolchain/MSVC.hpp"
#include "gubg/mss.hpp"
#include "gubg/std/filesystem.hpp"
#include <string>
#include <vector>

namespace cook { namespace model { 

    class Toolchain
    {
    public:
        struct ArchiveInfo
        {
            std::string cmd;
            std::string flags;
        };

        void add_highlights(std::filesystem::path &path) const
        {
            path /= std::filesystem::path(config_.arch);
            path /= std::filesystem::path(config_.config);
        }

        void set_config(const std::string &config) { config_.config = config; }
        void set_arch(const std::string &arch) { config_.arch = arch; }

        bool get_info(toolchain::Compiler::Ptr &compiler, toolchain::Linker::Ptr &linker, ArchiveInfo &archive) const
        {
            MSS_BEGIN(bool);
            if (false) {}
            else if (config_.arch == "uno")
            {
                /* compile.cmd_c = "avr-gcc"; */
                /* compile.cmd_cpp = "avr-g++ -std=c++17"; */
                /* compile.cmd_asm = "avr-as"; */
                /* compile.flags += " -flto"; */
                /* compile.defines += " -DARDUINO=10610"; */
                /* compile.defines += " -DARDUINO_ARCH_AVR"; */
                /* compile.defines += " -DARDUINO_AVR_UNO"; */
                /* compile.defines += " -DF_CPU=16000000L"; */

                /* link.cmd = "avr-g++ -std=c++17"; */
                /* link.flags += " -flto"; */

                archive.cmd = "avr-gcc-ar";
                archive.flags = "rcs";
            }
            else 
            {
                /* compiler.reset(new toolchain::gcc::Compiler(config_)); */
                compiler.reset(new toolchain::msvc::Compiler(config_));
                linker.reset(new toolchain::msvc::Linker(config_));
                /* compile.cmd_c = "gcc"; */
                /* compile.cmd_cpp = "g++ -std=c++17"; */
                /* compile.cmd_asm = "as"; */
                /* if (false) {} */
                /* else if (config_ == "release") { compile.defines += " -DNDEBUG"; } */
                /* else if (config_ == "debug") { compile.flags += " -g"; } */

                /* link.cmd = "g++ -std=c++17"; */

                if (false) {}
                else if (config_.arch == "x32")
                {
                    /* compile.flags += " -m32"; */
                    /* link.flags    += " -m32"; */
                }
                else if (config_.arch == "x64")
                {
                    /* compile.flags += " -m64"; */
                    /* link.flags    += " -m64"; */
                }

                archive.cmd = "ar";
                archive.flags = "rcs";
            }
            MSS_END();
        }

    private:
        toolchain::Config config_;
    };

} } 

#endif
