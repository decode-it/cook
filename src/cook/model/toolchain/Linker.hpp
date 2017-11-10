#ifndef HEADER_cook_model_toolchain_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_toolchain_Linker_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>
#include <memory>

namespace cook { namespace model { namespace toolchain { 

    using Executable = std::string;
    using ObjectFiles = std::vector<std::string>;

    class Linker
    {
    public:
        using Ptr = std::shared_ptr<Linker>;

        virtual std::string cmd_template(std::string executable,
                                         std::string objects) const = 0;
        virtual std::string prepare_executable(const Executable &) const = 0;
        virtual std::string prepare_objects(const ObjectFiles &) const = 0;
    };

} } } 

#endif
