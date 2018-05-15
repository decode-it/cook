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

    std::ostream & operator<<(std::ostream & oss, Content content);
    bool is_internal_generated(Content content);

}

#endif

