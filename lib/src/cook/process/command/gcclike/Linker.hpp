#ifndef HEADER_cook_process_command_gcclike_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_gcclike_Linker_hpp_ALREADY_INCLUDED

#include "cook/process/command/Linker.hpp"
#include <string>

namespace cook { namespace process { namespace command { namespace gcclike {

class Linker : public command::Linker
{
public:
    Linker(std::string cli);

    std::string name() const override;
    Result process(const Filenames & input_files, const Filenames & output_files) override;
    void stream_command(std::ostream & oss, const Filenames & input_files, const Filenames & output_files) const override;

    void add_library(const std::string &library_prefix) override;
    void add_library_path(const std::filesystem::path & path) override;

private:
    ArgumentDesc input_;
    ArgumentDesc output_;
    ArgumentDesc library_;
    ArgumentDesc library_path_;


};

} } } }

#endif
