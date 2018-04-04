#include "cook/log/level.hpp"

namespace cook { namespace log { 

    int &level_meyers()
    {
        static int level;
        static bool do_init = true;
        if (do_init)
        {
            do_init = false;
            level = 0;
        }
        return level;
    }

    int level()
    {
        return level_meyers();
    }
    void set_level(int level)
    {
        level_meyers() = level;
    }
    bool do_log(int importance)
    {
        return (importance+log::level()) >= 0;
    }

} } 
