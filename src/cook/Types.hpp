#ifndef HEADER_cook_Types_hpp_ALREADY_INCLUDED
#define HEADER_cook_Types_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>

#include "cook/File.hpp"
#include <functional>

namespace cook { 

    enum LogType
    {
        Message, Info, Error, Always,
    };

    using Strings = std::vector<std::string>;
    using FileFilterPredicate = std::function<bool (File &)>;
    enum class Overwrite {Never, IfSame, Always };

    inline std::ostream & operator<<(std::ostream & str, Overwrite value)
    {
        switch(value)
        {
    #define L_C(NAME, VALUE) case Overwrite::NAME: str << VALUE; break
        L_C(Never, "never");
        L_C(IfSame, "if_same");
        L_C(Always, "always");
    #undef CASES
        default:
            str << "Unknown";
        }
        return str;
    }

} 

#endif
