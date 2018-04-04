#ifndef HEADER_cook_log_Node_hpp_ALREADY_INCLUDED
#define HEADER_cook_log_Node_hpp_ALREADY_INCLUDED

#include "cook/log/level.hpp"
#include <string>
#include <memory>
#include <iostream>

namespace cook { namespace log { 

    class Node: public std::enable_shared_from_this<Node>
    {
    public:
        using Ptr = std::shared_ptr<Node>;

        static Ptr &top_ptr()
        {
            static Ptr ptr;
            if (!ptr)
                ptr.reset(new Node);
            return ptr;
        }
        static Node &top() {return *top_ptr();}

        Ptr add(const std::string &header, Importance importance = Importance{})
        {
            return Ptr(new Node(shared_from_this(), header, importance.value_or(importance_)));
        }

        Ptr parent() const {return parent_;}

        const std::string &header() const {return header_;}

        int importance() const {return importance_;}

    private:
        Node() {}
        Node(const Ptr &parent, const std::string &header, int importance): parent_(parent), header_(header), importance_(importance) {}

        Ptr parent_;
        std::string header_;
        int importance_ = 0;
    };

    inline int importance(Importance imp)
    {
        return imp.value_or(Node::top().importance());
    }

} } 

#endif
