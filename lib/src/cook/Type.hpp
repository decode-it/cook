#ifndef HEADER_cook_Type_hpp_ALREADY_INCLUDED
#define HEADER_cook_Type_hpp_ALREADY_INCLUDED

namespace cook {

enum class Type
{
    Undefined,
    Source,
    Header,
    ForceInclude,
    IncludePath,
    LibraryPath,
    Library,
    Define,
    Executable,

    UserDefined
};

}

#endif
