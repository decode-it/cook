#include "cook/Content.hpp"

namespace cook {

    std::ostream & operator<<(std::ostream & oss, Content content)
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

    bool is_internal_generated(Content content)
    {
        switch(content)
        {
            case Content::User:
            case Content::Globbing:
            default:
                return false;

            case Content::Generated:
                return true;
        }
    }
}
