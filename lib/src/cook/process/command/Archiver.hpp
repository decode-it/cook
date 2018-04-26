#ifndef HEADER_cook_process_command_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Archiver_hpp_ALREADY_INCLUDED

#include "cook/process/command/OrderedCommand.hpp"
#include <memory>

namespace cook { namespace process { namespace command {

class Archiver : public OrderedCommand
{
public:
    virtual Type type() const { return Archive; }
};

using ArchiverPtr = std::shared_ptr<Archiver>;

} } }

#endif
