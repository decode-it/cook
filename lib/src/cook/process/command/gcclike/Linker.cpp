#include "cook/process/command/gcclike/Linker.hpp"

namespace cook { namespace process { namespace command { namespace gcclike {

Linker::Linker(std::string cli)
    : input_(10),
      output_(20, "-o"),
      library_(30, "-l"),
      library_path_(40, "-L")
{  
    set_command(cli);
    add_argument(ArgumentDesc(0, "-std=c++17"), std::string());
}


void Linker::add_library(const std::string & library_prefix)
{
    add_argument(library_, library_prefix);
}

void Linker::add_library_path(const std::filesystem::path & path)
{
    add_argument(library_path_, path.string());
}

std::string Linker::name() const
{
    return gubg::stream([&](auto & os) { os << "linker [" << command()  << "]"; });
}

Result Linker::process(const Filenames & input_files, const Filenames & output_files)
{
    return Result();
}

void Linker::stream_command(std::ostream & oss, const Filenames & input_files, const Filenames & output_files) const
{
    OrderedCommand::to_stream(oss, input_, input_files, output_, output_files);
}

} } } }
