#ifndef HEADER_cook_Type_hpp_ALREADY_INCLUDED
#define HEADER_cook_Type_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

enum class Type
{
    Undefined,
    Source,
    Header,
    Export,
    Object,
    ForceInclude,
    IncludePath,
    LibraryPath,
    Library,
    FrameworkPath,
    Framework,
    Dependency,
    Define,
    Executable,
    Bundle,
    UserDefined
};

inline std::ostream & operator<<(std::ostream & os, Type type)
{
    switch(type)
    {
#define L_CASE(NAME) case Type::NAME: return os << #NAME
        L_CASE(Undefined);
        L_CASE(Source);
        L_CASE(Header);
        L_CASE(Export);
        L_CASE(Object);
        L_CASE(ForceInclude);
        L_CASE(IncludePath);
        L_CASE(LibraryPath);
        L_CASE(Library);
        L_CASE(FrameworkPath);
        L_CASE(Framework);
        L_CASE(Dependency);
        L_CASE(Define);
        L_CASE(Executable);
        L_CASE(Bundle);
#undef L_CASE
        default:
        return os << "UserDefined(" << (static_cast<unsigned int>(type) - static_cast<unsigned int>(Type::UserDefined)) << ")";
    }
}

}

#endif
