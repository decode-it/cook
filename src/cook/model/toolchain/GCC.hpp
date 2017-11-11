#ifndef HEADER_cook_model_toolchain_GCC_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_toolchain_GCC_hpp_ALREADY_INCLUDED

#include "cook/model/toolchain/Interfaces.hpp"
#include "cook/model/toolchain/Config.hpp"
#include <sstream>
#include <iostream>
#include <cassert>

namespace cook { namespace model { namespace toolchain { 

    namespace gcc { 

        inline void add_arch(Flags &flags, const std::string &arch)
        {
            if (false) {}
            else if (arch == "x32") { flags.push_back("-m32"); }
            else if (arch == "x64") { flags.push_back("-m64"); }
        }

        class Compiler: public toolchain::Compiler
        {
        public:
            Compiler(const Config &config): config_(config)
            {
                if (false) {}
                else if (config_.arch == "uno")
                {
                    cli_c = "avr-gcc";
                    cli_cpp = "avr-g++ -std=c++17";
                    cli_asm = "avr-as";
                    flags.push_back("-flto");
                    defines.push_back("-DARDUINO=10610");
                    defines.push_back("-DARDUINO_ARCH_AVR");
                    defines.push_back("-DARDUINO_AVR_UNO");
                    defines.push_back("-DF_CPU=16000000L");
                }
                add_arch(flags, config_.arch);
            }

            std::string cmd_template(std::string language, const TemplateStubs &stubs) const override
            {
                oss_.str("");
                if (false) {}
                else if (language == "c")
                {
                    oss_ << "gcc -c " << stubs.flags << " " << stubs.source << " -o " << stubs.object << " " << stubs.defines << " " << stubs.include_paths << " " << stubs.force_includes << " -MMD -MF " << stubs.depfile;
                }
                else if (language == "c++")
                {
                    oss_ << "g++ -std=c++17 -c " << stubs.flags << " " << stubs.source << " -o " << stubs.object << " " << stubs.defines << " " << stubs.include_paths << " " << stubs.force_includes << " -MMD -MF " << stubs.depfile;
                }
                else if (language == "asm")
                {
                    oss_ << "as";
                }
                return oss_.str();
            }
            std::string prepare_flags(const Flags &p_flags) const override
            {
                oss_.str("");
                for (auto flag: flags)
                    oss_ << " " << flag;
                for (auto flag: p_flags)
                    oss_ << " " << flag;
                return oss_.str();
            }
            std::string prepare_defines(const Defines &defines) const override
            {
                oss_.str("");
                for (auto def: defines)
                    oss_ << " -D" << def;
                return oss_.str();
            }
            std::string prepare_include_paths(const IncludePaths &include_paths) const override
            {
                oss_.str("");
                for (auto ip: include_paths)
                    oss_ << " -I " << ip;
                return oss_.str();
            }
            std::string prepare_force_includes(const ForceIncludes &force_includes) const override
            {
                oss_.str("");
                for (auto fi: force_includes)
                    oss_ << " -include " << fi;
                return oss_.str();
            }

        private:
            mutable std::ostringstream oss_;
            const Config config_;
            std::string cli_c = "gcc";
            std::string cli_cpp = "g++ -std=c++17";
            std::string cli_asm = "as";
            Flags flags;
            Defines defines;
        };

        class Linker: public toolchain::Linker
        {
        public:
            Linker(const Config &config): config_(config)
            {
                if (false) {}
                else if (config_.arch == "uno")
                {
                    cli = "avr-g++ -std=c++17";
                    flags.push_back("-flto");
                }
                add_arch(flags, config_.arch);
            }

            std::string cmd_template(const TemplateStubs &stubs) const override
            {
                oss_.str("");
                oss_ << "g++ -o " << stubs.executable << " " << stubs.flags << " " << stubs.objects << " " << stubs.library_paths << " " << stubs.libraries;
                return oss_.str();
            }
            std::string prepare_objects(const ObjectFiles &objects) const override
            {
                oss_.str("");
                for (auto obj: objects)
                    oss_ << " " << obj;
                return oss_.str();
            }
            std::string prepare_flags(const Flags &p_flags) const override
            {
                oss_.str("");
                for (auto flag: flags)
                    oss_ << " " << flag;
                for (auto flag: p_flags)
                    oss_ << " " << flag;
                return oss_.str();
            }
            std::string prepare_library_paths(const LibraryPaths &library_paths) const override
            {
                oss_.str("");
                for (auto path: library_paths)
                    oss_ << " -L" << path;
                return oss_.str();
            }
            std::string prepare_libraries(const Libraries &libraries) const override
            {
                oss_.str("");
                for (auto lib: libraries)
                    oss_ << " -l" << lib;
                return oss_.str();
            }

        private:
            mutable std::ostringstream oss_;
            const Config config_;
            std::string cli = "g++ -std=c++17";
            Flags flags;
        };

        class Archiver: public toolchain::Archiver
        {
        public:
            Archiver(const Config &config): config_(config)
            {
                if (false) {}
                else if (config_.arch == "uno")
                {
                    flags.push_back("-flto");
                }
            }

            std::string cmd_template(const TemplateStubs &stubs) const override
            {
                oss_.str("");
                oss_ << "ar rcs " << stubs.library << " " << stubs.flags << " " << stubs.objects;
                return oss_.str();
            }
            std::string prepare_objects(const ObjectFiles &objects) const override
            {
                oss_.str("");
                for (auto obj: objects)
                    oss_ << " " << obj;
                return oss_.str();
            }
            std::string prepare_flags(const Flags &p_flags) const override
            {
                oss_.str("");
                for (auto flag: flags)
                    oss_ << " " << flag;
                return oss_.str();
            }

        private:
            mutable std::ostringstream oss_;
            const Config config_;
            std::string cli;
            Flags flags;
        };

    } 

} } } 

#endif
