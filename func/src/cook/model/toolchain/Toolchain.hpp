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

            Config config;
            MSS(complete_(config));

            if (false) {}
            else if (config.arch == "uno")
            {
            }
            else 
            {
                if (false) {}
                else if (config.name == "msvc")
                {
                    compiler.reset(new toolchain::msvc::Compiler(config));
                    linker.reset(new toolchain::msvc::Linker(config));
                    archiver.reset(new toolchain::msvc::Archiver(config));
                }
                else if (config.name == "gcc")
                {
                    compiler.reset(new toolchain::gcc::Compiler(config));
                    linker.reset(new toolchain::gcc::Linker(config));
                    archiver.reset(new toolchain::gcc::Archiver(config));
                }
            }
            MSS_END();
        }

    private:
        bool complete_(Config &config) const
        {
            MSS_BEGIN(bool);
            config = config_;
            if (config.name.empty())
            {
#ifdef _MSC_VER
                config.name = "msvc";
#else
                config.name = "gcc";
#endif
            }
            if (config.arch.empty())
                config.arch = "x32";
            if (config.config.empty())
                config.config = "release";
            MSS_END();
        }
        toolchain::Config config_;
    };

} } } 

#endif
