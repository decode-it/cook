#include "cook/model/Recipe.hpp"

namespace cook { namespace model {

    std::ostream &operator<<(std::ostream &os, FileType ft)
    {
        switch (ft)
        {
#define L_CASE(name) case FileType::name: os << #name; break
            L_CASE(Unknown);
            L_CASE(Source);
            L_CASE(Header);
        }
        return os;
    }

} }

