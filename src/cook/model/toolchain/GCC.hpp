#ifndef HEADER_cook_model_toolchain_GCC_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_toolchain_GCC_hpp_ALREADY_INCLUDED

#include "cook/model/toolchain/Compiler.hpp"
#include "cook/model/toolchain/Linker.hpp"
#include "cook/model/toolchain/Config.hpp"
#include <sstream>

namespace cook { namespace model { namespace toolchain { 

    namespace gcc { 

        class Compiler: public toolchain::Compiler
        {
        public:
            Compiler(const Config &config): config_(config) {}

            std::string cmd_template(std::string language,
                                     std::string object,
                                     std::string source,
                                     std::string flags,
                                     std::string defines,
                                     std::string include_paths,
                                     std::string force_includes) const override
            {
                oss_.str("");
                if (false) {}
                else if (language == "c")
                {
                    oss_ << "gcc -c " << flags << " " << source << " -o " << object << " " << defines << " " << include_paths << " " << force_includes;
                }
                else if (language == "c++")
                {
                    oss_ << "g++ -std=c++17 -c " << flags << " " << source << " -o " << object << " " << defines << " " << include_paths << " " << force_includes;
                }
                else if (language == "asm")
                {
                    oss_ << "as";
                }
                return oss_.str();
            }
            std::string prepare_source(const Source &source) const override
            {
                oss_.str(""); oss_ << source; return oss_.str();
            }
            std::string prepare_object(const Object &object) const override
            {
                oss_.str(""); oss_ << object; return oss_.str();
            }
            std::string prepare_flags(const Flags &flags) const override
            {
                oss_.str("");
                for (auto flag: flags)
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
        };

    } 

} } } 

#endif
