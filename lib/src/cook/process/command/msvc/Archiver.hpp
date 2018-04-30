#ifndef HEADER_cook_process_command_msvc_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_msvc_Archiver_hpp_ALREADY_INCLUDED

#include "cook/process/command/Archiver.hpp"

namespace cook { namespace process { namespace command { namespace msvc { 

    class Archiver: public command::Archiver
    {
    public:
        Archiver();

        std::string name() const override;
        Result process(const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files) override;
        void stream_command(std::ostream & oss, const std::list<std::filesystem::path> & input_files, const std::list<std::filesystem::path> & output_files) const override;

    private:
        ArgumentDesc input_;
        ArgumentDesc output_;
    };

} } } } 

#endif
