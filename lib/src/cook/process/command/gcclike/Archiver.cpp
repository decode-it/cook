#include "cook/process/command/gcclike/Archiver.hpp"

namespace cook { namespace process { namespace command { namespace gcclike {

Archiver::Archiver()
    : input_(20),
      output_(10)
{
    set_command("ar");
    add_argument(ArgumentDesc(0, "crf"), std::string());
}

std::string Archiver::name() const
{
    return gubg::stream([&](auto & os) { os << "archiver [" << command()  << "]"; });
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
