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
    UserDefined
};

inline std::ostream & operator<<(std::ostream & os, TargetType type)
{
    switch(type)
    {
#define L_CASE(NAME) case TargetType::NAME: os << #NAME; return os
        L_CASE(Undefined);
        L_CASE(Archive);
        L_CASE(SharedLibrary);
        L_CASE(Executable);
#undef L_CASE
        default:
            break;
    }

    if (type >= TargetType::UserDefined)
        os << "UserDefined (" << static_cast<unsigned int>(type) << ")";
    else
        os << "unknown";

    return os;
}


}

#endif
