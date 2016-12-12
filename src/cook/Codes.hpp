#ifndef HEADER_cook_Codes_hpp_ALREADY_INCLUDED
#define HEADER_cook_Codes_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook { 

    enum class ReturnCode
    {
        OK, Error,
    };

    inline std::ostream &operator<<(std::ostream &os, ReturnCode rc)
    {
        switch (rc)
        {
#define L_CASE(name) case ReturnCode::name: os << #name; break
            L_CASE(OK);
            L_CASE(Error);
#undef L_CASE
            default:
            os << "Unknown code " << (int)rc;
            break;
        }
        return os;
    }

} 

#endif
