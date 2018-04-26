#include "cook/process/command/msvc/Archiver.hpp"

namespace cook { namespace process { namespace command { namespace msvc { 

Archiver::Archiver()
    : input_(20),
      output_(10, "/OUT:")
{
    set_command("lib");
    /* add_argument(ArgumentDesc(0, args), std::string()); */
}

std::string Archiver::name() const
{
    return "msvc-archiver";
}

Result Archiver::process(const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files)
{
    return Result();
}

void Archiver::to_stream(std::ostream & oss, const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files)
{
    OrderedCommand::to_stream(oss, input_, input_files, output_, output_files);
}

} } } } 

