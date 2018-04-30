#ifndef HEADER_cook_process_command_OrderedCommand_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_OrderedCommand_hpp_ALREADY_INCLUDED

#include "cook/process/command/Interface.hpp"
#include <list>
#include <map>
#include <ostream>

namespace cook { namespace process { namespace command {

    using order_type = int;
    struct ArgumentDesc
    {
        ArgumentDesc(order_type priority = 0, const std::string & option = std::string(), bool add_space = true)
        : priority(priority),
        option(option),
        add_space(add_space)
        {
        }

        order_type priority;
        std::string option;
        bool add_space;
    };
    inline bool operator<(const ArgumentDesc & lhs, const ArgumentDesc & rhs)
    {
        return std::make_pair(lhs.priority, lhs.option) < std::make_pair(rhs.priority, rhs.option);
    }

    using Filename = std::filesystem::path;
    using Filenames = std::list<Filename>;

    using FilenamesMap = std::map<ArgumentDesc, const Filenames *>;

    class OrderedCommand : public Interface
    {
    public:
        void add_argument(const ArgumentDesc & desc, const std::string & value);
        void set_command(const std::filesystem::path & command);
        const std::filesystem::path & command() const;

        template <typename Functor> void each_argument(Functor && functor) const
        {
            for(const auto & p : arguments_)
                for(const auto & a : p.second)
                    functor(p.first.option, p.first.add_space, a);
        }

        template <typename Functor> bool each_argument(Functor && functor, const std::pair<order_type, order_type> & range = std::pair<order_type, order_type>(0, std::numeric_limits<order_type>::max())) const
        {
            MSS_BEGIN(bool);
            MSS(range.first <= range.second - 1);


            auto first = arguments_.lower_bound(range.first);
            auto second = arguments_.upper_bound(range.second-1);

            for(const auto & p : gubg::make_range(first, second))
                for(const auto & a : p.second)
                    functor(p.first.option, p.first.add_space, a);

            MSS_END();
        }

        /* Deprecated, use FilenamesMap version instead. */
        void to_stream(std::ostream & oss, const ArgumentDesc &input_desc, const Filenames & input_files, const ArgumentDesc &output_desc, const Filenames & output_files) const;
        void to_stream(std::ostream & oss, const FilenamesMap &) const;

    private:
        std::map<ArgumentDesc, std::list<std::string>> arguments_;
        std::filesystem::path command_;
    };

} } }

#endif
