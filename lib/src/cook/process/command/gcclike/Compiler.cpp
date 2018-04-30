#include "cook/process/command/gcclike/Compiler.hpp"
#include "gubg/debug.hpp"

namespace cook { namespace process { namespace command { namespace gcclike {

Compiler::Compiler(std::string cli_c, std::string cli_cxx, Language language)
    : input_(10),
      output_(20, "-o"),
      depfile_(30, "-MMD -MF"),
      define_(40, "-D"),
      include_(50, "-I"),
      force_include_(60, "-include")
{
    switch (language)
    {
        case Language::CXX:
            set_command(cli_cxx);
            add_argument(ArgumentDesc(0, "-std=c++17"), std::string());
            break;
        case Language::C:
            set_command(cli_c);
            break;

    default:
        break;
    }

    add_argument(ArgumentDesc(2, "-c"), std::string());

    bool debug = false;
    if (debug)
    {
        add_argument(ArgumentDesc(1, "-g"), std::string());
    }
    else
    {
        add_argument(ArgumentDesc(1, "-O3"), std::string());
        add_define("NDEBUG");
    }
}

void Compiler::add_define(const std::string & name, const std::string & value)
{
    add_argument(define_, gubg::stream([&](auto & os) { os << name << "=" << value; }));
}
void Compiler::add_define(const std::string & name)
{
    add_argument(define_, name);
}

void Compiler::add_include_path(const std::filesystem::path &path)
{
    add_argument(include_, path.string());
}

void Compiler::add_force_include(const std::filesystem::path & path)
{
    add_argument(force_include_, path.string());
}

std::string Compiler::name() const
{
    return gubg::stream([&](auto & os) { os << "compiler [" << command()  << "]"; });
}

Result Compiler::process(const Filenames & input_files, const Filenames & output_files)
{
    S("");
    return Result();
}

void Compiler::create_depfiles_(Filenames &dst, const Filenames &src)
{
    dst = src;
    for (auto &fn: dst)
        fn += ".d";
}

void Compiler::stream_command(std::ostream & oss, const Filenames & input_files, const Filenames & output_files) const
{
    FilenamesMap fm;
    fm[input_] = &input_files;
    fm[output_] = &output_files;
    Filenames dep_files;
    create_depfiles_(dep_files, output_files);
    fm[depfile_] = &dep_files;
    OrderedCommand::to_stream(oss, fm);
}

std::string Compiler::depfile(const Filenames & input_files, const Filenames & output_files) const
{
    Filenames dep_files;
    create_depfiles_(dep_files, output_files);
    if (dep_files.empty())
        return "";
    return dep_files.front();
}

} } } }
