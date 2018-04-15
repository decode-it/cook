#ifndef HEADER_cook_process_command_gcclike_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_gcclike_Compiler_hpp_ALREADY_INCLUDED

#include "cook/process/command/Compiler.hpp"

namespace cook { namespace process { namespace command { namespace gcclike {

class Compiler : public command::Compiler
{
public:
    Compiler();

    std::string name() const override;
    Result process(const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files) override;
    void to_stream(std::ostream & oss, const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files) override;

    void add_define(const std::string & name, const std::string & value) override;
    void add_define(const std::string & name) override;
    void add_include_path(const std::filesystem::path & path) override;

private:
    ArgumentDesc input_;
    ArgumentDesc output_;
    ArgumentDesc define_;
    ArgumentDesc include_;


};

} } } }

#endif
