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

    //TODO: Add variadic templates to allow a more flexible scope
    /*
#include <type_traits>
#include <iostream>

template <typename Ftor, typename T>
void process(std::true_type, Ftor &&ftor, T v){ftor(v);}
template <typename Ftor, typename T>
void process(std::false_type, Ftor &&ftor, T v){}

template <typename Wanted, typename Ftor>
void iterate(Ftor &&ftor) { }
template <typename Wanted, typename Ftor, typename T, typename... Rest>
void iterate(Ftor &&ftor, T v, Rest... rest)
{
    using DoProcess = std::bool_constant<std::is_convertible<T,Wanted>::value>;
    process(DoProcess{}, ftor, v);
    iterate<Wanted>(ftor, rest...);
}

template <typename Wanted, typename... Rest>
void print(Rest... rest)
{
    iterate<Wanted>([](auto v){std::cout << v << std::endl;}, rest...);
}

template <typename... Rest>
void f(Rest... rest)
{
    print<std::string>(rest...);
    print<int>(rest...);
}

int main()
{
    f(42, "abc", 24);
    return 0;
}
     * */

} } 

#endif
