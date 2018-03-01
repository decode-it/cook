#ifndef HEADER_cook_build_Command_hpp_ALREADY_INCLUDED
#define HEADER_cook_build_Command_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "gubg/std/filesystem.hpp"
#include <memory>

namespace cook { namespace build {

struct Command
{
    virtual std::string name() const = 0;
    virtual Result process(const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files) = 0;
};

using CommandPtr = std::shared_ptr<Command>;


} }

#endif
