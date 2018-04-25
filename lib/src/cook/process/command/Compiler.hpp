#ifndef HEADER_cook_process_command_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Compiler_hpp_ALREADY_INCLUDED

#include "cook/process/command/OrderedCommand.hpp"

namespace cook { namespace process { namespace command {

class Compiler : public OrderedCommand
{
public:
    virtual void add_define(const std::string & name, const std::string & value) = 0;
    virtual void add_define(const std::string & name) = 0;
    virtual void add_include_path(const std::filesystem::path & path) = 0;
    virtual void add_force_include(const std::filesystem::path & path) = 0;

    virtual Type type() const { return Compile; }
};


} } }

#endif
