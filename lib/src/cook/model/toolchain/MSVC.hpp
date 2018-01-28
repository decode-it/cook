#ifndef HEADER_cook_model_toolchain_MSVC_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_toolchain_MSVC_hpp_ALREADY_INCLUDED

#include "cook/model/toolchain/Interfaces.hpp"
#include "cook/model/toolchain/Config.hpp"
#include "cook/model/toolchain/Convert.hpp"
#include <sstream>

namespace cook { namespace model { namespace toolchain { 

    namespace msvc { 

        class Compiler: public toolchain::Compiler
        {
        public:
            Compiler(const Config &config): config_(config) {}

            std::string cmd_template(std::string language, const TemplateStubs &stubs) const override
            {
                oss_.str("");
                if (false) {}
                else if (language == "c")
                {
                    oss_ << "cl /nologo -c /Tc " << stubs.flags << " " << stubs.source << " /Fo:" << stubs.object << " " << stubs.defines << " " << stubs.include_paths << " " << stubs.force_includes;
                }
                else if (language == "c++")
                {
                    oss_ << "cl /nologo /EHsc -c " << stubs.flags << " " << stubs.source << " /Fo:" << stubs.object << " " << stubs.defines << " " << stubs.include_paths << " " << stubs.force_includes;
                }
                else if (language == "asm")
                {
                    oss_ << "<not present>";
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
                auto stream = [&](const auto & p) { oss_ << " -D" << p.first << "=" << p.second; };


                for (auto def: defines)
                    stream(def);

                return oss_.str();
            }
            std::string create_define(const std::string & name, const std::string & value) const override
            {
                if (value.empty())
                    return name;
                else
                    return name + "=" + value;
            }
            std::string prepare_include_paths(const IncludePaths &include_paths) const override
            {
                oss_.str("");
                for (auto ip: include_paths)
                    oss_ << " /I" << ip;
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

        class Linker: public toolchain::Linker
        {
        public:
            Linker(const Config &config): config_(config) {}

            std::string cmd_template(const TemplateStubs &stubs) const override
            {
                oss_.str("");
                oss_ << "link /OUT:" << stubs.executable << " " << stubs.flags << " " << stubs.objects << " " << stubs.libraries;
                return oss_.str();
            }
            std::string prepare_objects(const ObjectFiles &objects) const override
            {
                oss_.str("");
                for (auto obj: objects)
                    oss_ << " " << obj;
                return oss_.str();
            }
            std::string prepare_flags(const Flags &flags) const override
            {
                oss_.str("");
                for (auto flag: flags)
                    oss_ << " " << flag;
                return oss_.str();
            }
            std::string prepare_library_paths(const LibraryPaths &library_paths) const override
            {
                oss_.str("");
                for (auto path: library_paths)
                    oss_ << " /LIBPATH:" << path;
                return oss_.str();
            }
            std::string prepare_libraries(const Libraries &libraries) const override
            {
                oss_.str("");
                for (auto lib: libraries)
                    oss_ << " " << convert_uri(lib) << ".lib";
                return oss_.str();
            }

        private:
            mutable std::ostringstream oss_;
            const Config config_;
        };

        class Archiver: public toolchain::Archiver
        {
        public:
            Archiver(const Config &config): config_(config) {}

            std::string cmd_template(const TemplateStubs &stubs) const override
            {
                oss_.str("");
                oss_ << "lib /OUT:" << stubs.library << " " << stubs.flags << " " << stubs.objects;
                return oss_.str();
            }
            std::string prepare_library(const Library &name) const override
            {
                oss_.str("");
                oss_ << convert_uri(name) << ".lib";
                return oss_.str();
            }
            std::string prepare_objects(const ObjectFiles &objects) const override
            {
                oss_.str("");
                for (auto obj: objects)
                    oss_ << " " << obj;
                return oss_.str();
            }
            std::string prepare_flags(const Flags &flags) const override
            {
                oss_.str("");
                for (auto flag: flags)
                    oss_ << " " << flag;
                return oss_.str();
            }

        private:
            mutable std::ostringstream oss_;
            const Config config_;
        };

    } 

} } } 

#endif
