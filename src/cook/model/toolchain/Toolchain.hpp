#ifndef HEADER_cook_model_toolchain_Toolchain_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_toolchain_Toolchain_hpp_ALREADY_INCLUDED

#include "cook/model/toolchain/GCC.hpp"
#include "cook/model/toolchain/MSVC.hpp"
#include "gubg/mss.hpp"
#include "gubg/std/filesystem.hpp"
#include <string>
#include <vector>

namespace cook { namespace model { namespace toolchain { 

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

        void set_name(const std::string &name) { config_.name = name; }
        void set_config(const std::string &config) { config_.config = config; }
        void set_arch(const std::string &arch) { config_.arch = arch; }

        bool get_info(Compiler::Ptr &compiler, Linker::Ptr &linker, Archiver::Ptr &archiver) const
        {
            MSS_BEGIN(bool);

            auto name = config_.name;
            if (name.empty())
            {
#ifdef _MSC_VER
                name = "msvc";
#else
                name = "gcc";
#endif
            }

            if (false) {}
            else if (config_.arch == "uno")
            {
            }
            else 
            {
                if (false) {}
                else if (name == "msvc")
                {
                    compiler.reset(new toolchain::msvc::Compiler(config_));
                    linker.reset(new toolchain::msvc::Linker(config_));
                    archiver.reset(new toolchain::msvc::Archiver(config_));
                }
                else if (name == "gcc")
                {
                    compiler.reset(new toolchain::gcc::Compiler(config_));
                    linker.reset(new toolchain::gcc::Linker(config_));
                    archiver.reset(new toolchain::gcc::Archiver(config_));
                }
            }
            MSS_END();
        }

    private:
        toolchain::Config config_;
    };

} } } 

#endif
