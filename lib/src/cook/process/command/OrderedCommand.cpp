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

#if 1
void OrderedCommand::to_stream(std::ostream & oss, const ArgumentDesc & input_desc, const Filenames & input_files, const ArgumentDesc & output_desc, const Filenames & output_files) const
{
    bool input_first = input_desc.priority <= output_desc.priority;
    const ArgumentDesc & f1 = input_first ? input_desc : output_desc;
    const ArgumentDesc & f2 = input_first ? output_desc : input_desc;
    const Filenames & l1 = input_first ? input_files : output_files;
    const Filenames & l2 = input_first ? output_files : input_files;

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
#endif
void OrderedCommand::to_stream(std::ostream & oss, const FilenamesMap &filenames_map) const
{
    auto to_stream = [&](const std::string & option, bool add_space, const std::string & value)
    {
        if (!option.empty())
            oss << option << (add_space ? " " : "");
        if (!value.empty())
            oss << value << " ";
    };
    auto stream_files = [&](const ArgumentDesc & argdesc, const Filenames & filenames)
    {
        for(const auto & f : filenames)
        {
            if (!argdesc.option.empty())
                oss << argdesc.option << " ";

            oss << f.string() << " ";
        }
    };

    oss << command_.string() << " ";

    int prev_prio = 0;
    for (const auto &p: filenames_map)
    {
        const auto &argdesc = p.first;
        const auto &filenames = *p.second;

        each_argument(to_stream, std::make_pair(prev_prio, argdesc.priority));
        stream_files(argdesc, filenames);

        prev_prio = argdesc.priority;
    }

    each_argument(to_stream, std::make_pair(prev_prio, std::numeric_limits<order_type>::max()));
}

} } }


