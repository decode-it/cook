#ifndef HEADER_cook_Type_hpp_ALREADY_INCLUDED
#define HEADER_cook_Type_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

enum class Type
{
    Undefined,
    Source,
    Header,
    Object,
    ForceInclude,
    IncludePath,
    LibraryPath,
    Library,
    Define,
    Executable,

    UserDefined
};

inline std::ostream & operator<<(std::ostream & os, Type type)
{
    switch(type)
    {
#define L_CASE(NAME) case Type::NAME: os << #NAME; return os
        L_CASE(Undefined);
        L_CASE(Source);
        L_CASE(Header);
        L_CASE(Object);
        L_CASE(ForceInclude);
        L_CASE(IncludePath);
        L_CASE(LibraryPath);
        L_CASE(Library);
        L_CASE(Define);
        L_CASE(Executable);
#undef L_CASE
        default:
            break;
    }

    if (type >= Type::UserDefined)
        os << "UserDefined (" << static_cast<unsigned int>(type) << ")";
    else
        os << "unknown";

    return os;
}

}

#endif
