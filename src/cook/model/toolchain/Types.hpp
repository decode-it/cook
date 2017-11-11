#ifndef HEADER_cook_model_toolchain_Types_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_toolchain_Types_hpp_ALREADY_INCLUDED

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
    using Libraries = std::vector<Library>;
    using LibraryPaths = std::vector<std::string>;

} } } 

#endif
