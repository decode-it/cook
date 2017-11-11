#ifndef HEADER_cook_model_toolchain_Interfaces_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_toolchain_Interfaces_hpp_ALREADY_INCLUDED

#include "cook/model/toolchain/Toolchain.hpp"
#include <string>
#include <vector>
#include <memory>

namespace cook { namespace model { namespace toolchain { 

    using Object = std::string;
    using Source = std::string;
    using DepFile = std::string;
    using Executable = std::string;
    using Library = std::string;

    using Flags = std::vector<std::string>;
    using Defines = std::vector<std::string>;
    using IncludePaths = std::vector<std::string>;
    using ForceIncludes = std::vector<std::string>;
    using ObjectFiles = std::vector<std::string>;

    class Compiler
    {
    public:
        using Ptr = std::shared_ptr<Compiler>;

        struct TemplateStubs
        {
            std::string object;
            std::string source;
            std::string depfile;
            std::string flags;
            std::string defines;
            std::string include_paths;
            std::string force_includes;
        };
        virtual std::string cmd_template(std::string language, const TemplateStubs &) const = 0;
        virtual std::string prepare_object(const Object &fn) const {return fn;}
        virtual std::string prepare_source(const Source &fn) const {return fn;}
        virtual std::string prepare_depfile(const DepFile &fn) const {return fn;}
        virtual std::string prepare_flags(const Flags &) const = 0;
        virtual std::string prepare_defines(const Defines &) const = 0;
        virtual std::string prepare_include_paths(const IncludePaths &) const = 0;
        virtual std::string prepare_force_includes(const ForceIncludes &) const = 0;
    };

    class Linker
    {
    public:
        using Ptr = std::shared_ptr<Linker>;

        struct TemplateStubs
        {
            std::string executable;
            std::string objects;
            std::string flags;
        };
        virtual std::string cmd_template(const TemplateStubs &) const = 0;

        virtual std::string prepare_executable(const Executable &fn) const {return fn;}
        virtual std::string prepare_objects(const ObjectFiles &) const = 0;
        virtual std::string prepare_flags(const Flags &) const = 0;
    };

    class Archiver
    {
    public:
        using Ptr = std::shared_ptr<Archiver>;

        struct TemplateStubs
        {
            std::string library;
            std::string objects;
            std::string flags;
        };
        virtual std::string cmd_template(const TemplateStubs &) const = 0;

        virtual std::string prepare_library(const Library &fn) const {return fn;}
        virtual std::string prepare_objects(const ObjectFiles &) const = 0;
        virtual std::string prepare_flags(const Flags &) const = 0;
    };

} } } 

#endif
