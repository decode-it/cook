#ifndef HEADER_cook_chai_Logger_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Logger_hpp_ALREADY_INCLUDED

#include "cook/Logger.hpp"

namespace cook { namespace chai {

class Logger : public cook::Logger
{
public:
    void log(MessageType type, const LogFunction & function) override;
};

} }

#endif
