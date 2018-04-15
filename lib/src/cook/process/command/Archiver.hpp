#ifndef HEADER_cook_process_command_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Archiver_hpp_ALREADY_INCLUDED

#include "cook/process/command/OrderedCommand.hpp"

namespace cook { namespace process { namespace command {

class Archiver : public OrderedCommand
{
public:
    virtual Type type() const { return Archive; }
};

} } }

#endif
