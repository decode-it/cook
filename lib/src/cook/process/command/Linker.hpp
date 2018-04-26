#ifndef HEADER_cook_process_command_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Linker_hpp_ALREADY_INCLUDED

#include "cook/process/command/OrderedCommand.hpp"
#include <memory>

namespace cook { namespace process { namespace command {

class Linker : public OrderedCommand
{
public:
    virtual void add_library(const std::filesystem::path & library_prefix) = 0;
    virtual void add_library_path(const std::filesystem::path & path) = 0;

    Type type() const override { return Link; }
};

using LinkerPtr = std::shared_ptr<Linker>;

} } }

#endif
