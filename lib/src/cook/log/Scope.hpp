#ifndef HEADER_cook_log_Scope_hpp_ALREADY_INCLUDED
#define HEADER_cook_log_Scope_hpp_ALREADY_INCLUDED

#include "cook/log/Node.hpp"
#include <string>
#include <sstream>

namespace cook { namespace log { 

    namespace details { 
        class Header
        {
        public:
            Header(std::ostringstream &oss): oss_(oss) {}

            void tag(const std::string &name) {oss_ << "[" << name << "]";}

            template <typename Key>
            Header &attr(const Key &key)
            {
                oss_ << "(" << key << ")";
                return *this;
            }
            template <typename Key, typename Value>
            Header &attr(const Key &key, const Value &value)
            {
                oss_ << "(" << key << ":" << value << ")";
                return *this;
            }

        private:
            std::ostringstream &oss_;
        };
    } 

    class Scope
    {
    public:
        Scope(const Node::Ptr &node);
        Scope(Scope &&dying);
        ~Scope();

    private:
        Scope(const Scope &);
        Scope &operator=(const Scope &);

        static std::ostream &indent_(std::ostream &os, bool increase);

        Node::Ptr node_;
        bool do_log_ = false;
        bool is_open_ = true;
    };

    Scope scope(const std::string &tag, Importance importance);
    Scope scope(const std::string &tag);

    template <typename Ftor>
    Scope scope(const std::string &tag, Importance importance, Ftor &&ftor)
    {
        static std::ostringstream oss;
        oss.str("");
        details::Header header(oss);
        header.tag(tag);
        ftor(header);
        return Scope{Node::top().add(oss.str(), importance)};
    }
    template <typename Ftor>
    Scope scope_(const std::string &tag, Ftor &&ftor) { return scope(tag, Importance{}, ftor); }

} } 

#endif
