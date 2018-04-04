#include "cook/log/Scope.hpp"

namespace cook { namespace log { 

    Scope::Scope(const Node::Ptr &node): node_(node), do_log_(log::do_log(node_->importance()))
    {
        if (do_log_)
            indent_(std::cout, true) << node_->header();
        Node::top_ptr() = node_;
    }
    Scope::Scope(Scope &&dying): do_log_(dying.do_log_)
    {
        std::swap(node_, dying.node_);
    }
    Scope::~Scope()
    {
        if (!node_)
            //Dying
            return;
        if (do_log_)
            indent_(std::cout, false);
        Node::top_ptr() = node_->parent();
    }

    std::ostream &Scope::indent_(std::ostream &os, bool increase)
    {
        static unsigned int level = 0;
        static bool is_open = true;
        static std::string str;
        if (increase)
        {
            str.resize(level*2, ' ');
            if (level > 0)
            {
                if (is_open)
                    os << "{";
                os << std::endl << str;
            }
            is_open = true;
            ++level;
        }
        else
        {
            --level;
            str.resize(level*2, ' ');
            if (!is_open)
                os << std::endl << str << "}";
            is_open = false;
        }
        return os;
    }

    Scope scope(const std::string &tag, Importance importance)
    {
        static std::ostringstream oss;
        oss.str("");
        details::Header header(oss);
        header.tag(tag);
        return Scope{Node::top().add(oss.str(), importance)};
    }
    Scope scope(const std::string &tag) { return scope(tag, Importance{}); }

} } 
