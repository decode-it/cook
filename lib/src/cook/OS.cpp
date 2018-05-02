#include "cook/OS.hpp"
#include "boost/predef.h"

namespace cook {

OS get_os()
{
#if BOOST_OS_LINUX
    return OS::Linux;
#endif

#if BOOST_OS_WINDOWS
    return OS::Windows;
#endif

#if BOOST_OS_MACOS
    return OS::MacOS;
#endif
}

std::ostream & operator<<(std::ostream & str, OS os)
{
    switch(os)
    {
#define L_CASE(NAME) case OS::NAME: return str << #NAME
        L_CASE(Linux);
        L_CASE(Windows);
        L_CASE(MacOS);
#undef L_CASE
        default:
            return str << "<unknown>";
    }
}

}
