#ifndef HEADER_cook_process_command_msvc_Linker_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_msvc_Linker_hpp_ALREADY_INCLUDED

#include "cook/process/command/Linker.hpp"

namespace cook { namespace process { namespace command { namespace msvc { 

    class Linker: public command::Linker
    {
    public:
        Linker();

        std::string name() const override;
        Result process(const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files) override;
        void to_stream(std::ostream & oss, const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files) override;

        void add_library(const std::string & library_prefix) override;
        void add_library_path(const std::filesystem::path & path) override;

    private:
        ArgumentDesc input_;
        ArgumentDesc output_;
        ArgumentDesc library_;
        ArgumentDesc library_path_;
    private:
    };

} } } } 

#endif
