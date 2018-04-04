#ifndef HEADER_cook_log_level_hpp_ALREADY_INCLUDED
#define HEADER_cook_log_level_hpp_ALREADY_INCLUDED

#include <optional>

namespace cook { namespace log { 

    using Importance = std::optional<int>;

    int level();
    void set_level(int level);
    bool do_log(int importance);

} } 

#endif
