#ifndef HEADER_cook_Types_hpp_ALREADY_INCLUDED
#define HEADER_cook_Types_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>

#include "cook/File.hpp"
#include <functional>

namespace cook { 

using FileFilterPredicate = std::function<bool (File &)>;

enum class Propagation
{
    Local = 0,
    Global = 1,
};
inline std::ostream & operator<<(std::ostream & str, Propagation propagation)
{
    switch (propagation)
    {
#define L_C(NAME, VALUE) case Propagation::NAME: str << VALUE; break
        L_C(Local, "local");
        L_C(Global, "global");
        default:
            str << "Unknown";
    }
#undef L_C
    return str;
}

enum class Overwrite {Never, IfSame, Always };
inline std::ostream & operator<<(std::ostream & str, Overwrite value)
{
    switch(value)
    {
#define L_C(NAME, VALUE) case Overwrite::NAME: str << VALUE; break
        L_C(Never, "never");
        L_C(IfSame, "if_same");
        L_C(Always, "always");
#undef L_C
        default:
            str << "Unknown";
    }
    return str;
}

} 

#endif
