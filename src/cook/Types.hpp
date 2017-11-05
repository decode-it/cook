#ifndef HEADER_cook_Types_hpp_ALREADY_INCLUDED
#define HEADER_cook_Types_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>

namespace cook { 

    enum LogType
    {
        Message, Info, Error,
    };

    using Strings = std::vector<std::string>;

} 

#endif
