#ifndef HEADER_cook_process_command_PriorityBased_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_PriorityBased_hpp_ALREADY_INCLUDED

#include "cook/process/command/Interface.hpp"
#include <list>
#include <map>
#include <ostream>

namespace cook { namespace process { namespace command {

class OrderedCommand : public Interface
{
public:
    using order_type = unsigned char;

    struct ArgumentDesc
    {
        ArgumentDesc(order_type priority = 0, const std::string & option = std::string())
            : priority(priority),
              option(option)
        {
        }

        order_type priority;
        std::string option;
    };

    void add_argument(const ArgumentDesc & desc, const std::string & value);
    void set_command(const std::filesystem::path & command);
    const std::filesystem::path & command() const;

    template <typename Functor> void each_argument(Functor && functor) const
    {
        for(const auto & p : arguments_)
            for(const auto & a : p.second)
                functor(p.first.option, a);
    }

    template <typename Functor> bool each_argument(Functor && functor, const std::pair<order_type, order_type> & range = std::pair<order_type, order_type>(0, std::numeric_limits<order_type>::max())) const
    {
        MSS_BEGIN(bool);
        MSS(range.first <= range.second - 1);


        auto first = arguments_.lower_bound(range.first);
        auto second = arguments_.upper_bound(range.second-1);

        for(const auto & p : gubg::make_range(first, second))
            for(const auto & a : p.second)
                functor(p.first.option, a);
    }

    void to_stream(std::ostream & oss, const ArgumentDesc &input_desc, const std::list<std::filesystem::path> & input_files, const ArgumentDesc &output_desc, const std::list<std::filesystem::path> & output_files) const;

private:
    std::map<ArgumentDesc, std::list<std::string>> arguments_;
    std::filesystem::path command_;
};

inline bool operator<(const OrderedCommand::ArgumentDesc & lhs, const OrderedCommand::ArgumentDesc & rhs)
{
    return lhs.priority < rhs.priority || (lhs.priority == rhs.priority && lhs.option < rhs.option);
}



} } }

#endif
