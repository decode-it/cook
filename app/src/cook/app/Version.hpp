#ifndef HEADER_cook_app_Version_hpp_ALREADY_INCLUDED
#define HEADER_cook_app_Version_hpp_ALREADY_INCLUDED

#include <string>

#define COOK_VERSION_MAJOR 1
#define COOK_VERSION_MINOR 1
#define COOK_VERSION_PATCH 1

#define COOK_XSTR(A) COOK_STR(A)
#define COOK_STR(A) #A

namespace cook { namespace app {

inline const std::string & version()
{
    static std::string version;
    if (version.empty())
    {
        version = COOK_XSTR(COOK_VERSION_MAJOR) "." COOK_XSTR(COOK_VERSION_MINOR) "." COOK_XSTR(COOK_VERSION_PATCH);
#ifdef NDEBUG
        version += " (release build)";
#else
        version += " (debug build)";
#endif
    }
    return version;
}

} }




#endif // VERSION_HPP
