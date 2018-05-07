#ifndef HEADER_cook_Content_hpp_ALREADY_INCLUDED
#define HEADER_cook_Content_hpp_ALREADY_INCLUDED

#include <ostream>

namespace cook {

    enum class Content
    {
        User,
        Globbing,
        Generated
    };

    inline std::ostream & operator<<(std::ostream & oss, Content content)
    {
        switch(content)
        {
#define L_CASE(NAME) case Content::NAME: return oss << #NAME
            L_CASE(User);
            L_CASE(Globbing);
            L_CASE(Generated);
#undef L_CASE
            default:
            return oss << "undefined>";
        }
    }

}

#endif

