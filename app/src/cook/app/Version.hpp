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
        version += " (";
#ifdef NDEBUG
        version += "release";
#else
        version += "debug";
#endif
        version += ", ";
        const auto nr_bits = (sizeof(void*)*8);
        version += std::to_string(nr_bits);
        version += "bit)";
    }
    return version;
}

} }




#endif // VERSION_HPP
