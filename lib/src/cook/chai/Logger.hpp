#ifndef HEADER_cook_chai_Logger_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Logger_hpp_ALREADY_INCLUDED

#include "cook/Logger.hpp"

namespace cook { namespace chai {

class Logger : public cook::Logger
{
public:
    void log(const Result & result) const override;
};

} }

#endif
