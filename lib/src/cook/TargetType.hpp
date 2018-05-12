#ifndef HEADER_cook_TargetType_hpp_ALREADY_INCLUDED
#define HEADER_cook_TargetType_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

enum class TargetType
{
    Undefined,
    Archive,
    SharedLibrary,
    Executable,
    Script,
    UserDefined
};

inline std::ostream & operator<<(std::ostream & os, TargetType type)
{
    switch(type)
    {
#define L_CASE(NAME) case TargetType::NAME: return os << #NAME
        L_CASE(Undefined);
        L_CASE(Archive);
        L_CASE(SharedLibrary);
        L_CASE(Executable);
        L_CASE(Script);
#undef L_CASE
        default:
        return os << "UserDefined(" << (static_cast<unsigned int>(type) - static_cast<unsigned int>(TargetType::UserDefined)) << ")";
    }
}


}

#endif
