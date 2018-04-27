#include "cook/process/command/OrderedCommand.hpp"

namespace cook { namespace process { namespace command {

void OrderedCommand::add_argument(const ArgumentDesc & desc, const std::string & value)
{
    arguments_[desc].push_back(value);
}

void OrderedCommand::set_command(const std::filesystem::path & command)
{
    command_ = command;
}

const std::filesystem::path & OrderedCommand::command() const
{
    return command_;
}

void OrderedCommand::to_stream(std::ostream & oss, const ArgumentDesc & input_desc, const std::list<std::filesystem::path> & input_files, const ArgumentDesc & output_desc, const std::list<std::filesystem::path> & output_files) const
{
    bool input_first = input_desc.priority <= output_desc.priority;
    const ArgumentDesc & f1 = input_first ? input_desc : output_desc;
    const ArgumentDesc & f2 = input_first ? output_desc : input_desc;
    const std::list<std::filesystem::path> & l1 = input_first ? input_files : output_files;
    const std::list<std::filesystem::path> & l2 = input_first ? output_files : input_files;



    auto to_stream = [&](const std::string & option, bool add_space, const std::string & value)
    {
        if (!option.empty())
            oss << option << (add_space ? " " : "");
        if (!value.empty())
            oss << value << " ";
    };

    auto stream_files = [&](const ArgumentDesc & desc, const std::list<std::filesystem::path> & files)
    {
        for(const auto & f : files)
        {
            if (!desc.option.empty())
                oss << desc.option << " ";

            oss << f.string() << " ";
        }
    };

    oss << command_.string() << " ";

    // the first set of arguments
    each_argument(to_stream, std::make_pair(0, f1.priority));
    stream_files(f1, l1);
    each_argument(to_stream, std::make_pair(f1.priority, f2.priority));
    stream_files(f2, l2);
    each_argument(to_stream, std::make_pair(f2.priority, std::numeric_limits<order_type>::max()));
}


} } }


