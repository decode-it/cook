#ifndef HEADER_cook_model_toolchain_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_toolchain_Compiler_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>
#include <memory>

namespace cook { namespace model { namespace toolchain { 

    using Object = std::string;
    using Source = std::string;
    using Flags = std::vector<std::string>;
    using Defines = std::vector<std::string>;
    using IncludePaths = std::vector<std::string>;
    using ForceIncludes = std::vector<std::string>;

    class Compiler
    {
    public:
        using Ptr = std::shared_ptr<Compiler>;

        virtual std::string cmd_template(std::string language,
                                         std::string object,
                                         std::string source,
                                         std::string flags,
                                         std::string defines,
                                         std::string include_paths,
                                         std::string force_includes) const = 0;
        virtual std::string prepare_object(const Object &) const = 0;
        virtual std::string prepare_source(const Source &) const = 0;
        virtual std::string prepare_flags(const Flags &) const = 0;
        virtual std::string prepare_defines(const Defines &) const = 0;
        virtual std::string prepare_include_paths(const IncludePaths &) const = 0;
        virtual std::string prepare_force_includes(const ForceIncludes &) const = 0;
    };

} } } 

#endif
