#ifndef HEADER_cook_Version_hpp_ALREADY_INCLUDED
#define HEADER_cook_Version_hpp_ALREADY_INCLUDED

#include <string>

#define COOK_VERSION_MAJOR 1
#define COOK_VERSION_MINOR 2
#define COOK_VERSION_PATCH 23

#define COOK_XSTR(A) COOK_STR(A)
#define COOK_STR(A) #A

namespace cook {

    inline const std::string & version()
    {
        static std::string version_;

        if (version_.empty())
            version_ = COOK_XSTR(COOK_VERSION_MAJOR) "." COOK_XSTR(COOK_VERSION_MINOR) "." COOK_XSTR(COOK_VERSION_PATCH);

        return version_;
    }

    inline const std::string & extended_version()
    {
        static std::string version_;
        if (version_.empty())
        {
                version_ = version();
            version_ += " (";
#ifdef NDEBUG
            version_ += "release";
#else
            version_ += "debug";
#endif
            version_ += ", ";
            const auto nr_bits = (sizeof(void*)*8);
            version_ += std::to_string(nr_bits);
            version_ += "bit)";
        }
        return version_;
    }
}

#endif 
