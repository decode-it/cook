#ifndef HEADER_cook_OS_hpp_ALREADY_INCLUDED
#define HEADER_cook_OS_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

enum class OS
{
    Linux,
    Windows,
    MacOS
};

std::ostream & operator<<(std::ostream & str, OS os);
OS get_os();

}

#endif
