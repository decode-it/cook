#include "cook/Hook.hpp"

namespace cook { 

    std::ostream & operator<<(std::ostream & str, Hook hook)
    {
        switch(hook)
        {
#define L_CASE(NAME) case Hook::NAME: return str << #NAME
            L_CASE(Pre);
            L_CASE(Post);
#undef L_CASE
            default:
            return str << "<unknown>";
        }
    }
}
