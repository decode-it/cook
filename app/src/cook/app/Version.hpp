#ifndef HEADER_cook_app_Version_hpp_ALREADY_INCLUDED
#define HEADER_cook_app_Version_hpp_ALREADY_INCLUDED

#include <string>

#define COOK_VERSION_MAJOR 1
#define COOK_VERSION_MINOR 1
#define COOK_VERSION_PATCH 0

namespace cook { namespace app {

inline const std::string & version()
{
    static std::string version = "1.1.0";
    return version;
}

} }




#endif // VERSION_HPP
