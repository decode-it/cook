#ifndef HEADER_cook_View_hpp_ALREADY_INCLUDED
#define HEADER_cook_View_hpp_ALREADY_INCLUDED

#include "cook/Logger.hpp"
#include "cook/Handle.h"
#include "cook/view/Logger.hpp"
#include <iostream>

namespace cook { 

class View
{
public:
    cook::Logger logger() { return cook::Logger(&view::log_callback, &logger_); }

    std::ostream &log()                             { return logger_.log(); }
    std::ostream &log(cook_NotificationType_t lt)   { return logger_.log(lt); }

    bool process_cli(cook_Handle_t api, int argc, const char **argv);

    view::Logger logger_;
};

} 

#endif
