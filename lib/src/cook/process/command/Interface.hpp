#ifndef HEADER_cook_process_command_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Interface_hpp_ALREADY_INCLUDED

#include "cook/Result.hpp"
#include "gubg/std/filesystem.hpp"
#include <string>
#include <memory>

namespace cook { namespace process { namespace command {

class Interface
{
public:
    virtual ~Interface() {}

    virtual std::string name() const = 0;
    virtual Result process(const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files) = 0;
};

using Ptr = std::shared_ptr<Interface>;

} } }

#endif
