#ifndef HEADER_cook_log_Scope_hpp_ALREADY_INCLUDED
#define HEADER_cook_log_Scope_hpp_ALREADY_INCLUDED

#include "gubg/naft/Node.hpp"
#include "gubg/naft/Document.hpp"
#include "gubg/debug.hpp"
#include <iostream>
#include <optional>

namespace cook { namespace log { 

    class Scope
    {
    public:
        Scope(int level, gubg::naft::Node &node)
            : level_(level),
              node_(node)
        {
            parent_ = top_;
            top_ = this;
        }
        Scope(int level, gubg::naft::Node &node, const std::string &tag)
            : level_(level),
              node_opt_{node.node(tag)},
              node_(*node_opt_)
        {
            parent_ = top_;
            top_ = this;
        }
        Scope(Scope &&dying): level_(dying.level_), node_opt_(std::move(dying.node_opt_)), node_(!!node_opt_ ? *node_opt_ : dying.node_), parent_(dying.parent_)
        {
            top_ = this;
        }
        ~Scope()
        {
            if (top_ == this)
                top_ = parent_;
        }

        static Scope &top()
        {
            if (!top_)
            {
                auto *doc = new gubg::naft::Document(std::cout);
                top_ = new Scope(0, *doc);
            }
            return *top_;
        }

        void set_level(int level) {level_ = level;}

        //Indicates if level should be logged
        bool operator()(int level) const {return level_ >= level;}

        Scope scope(const std::string &tag, int level) const
        {
            if (!operator()(level))
                return Scope(level_, node_);

            return Scope(level_, node_, tag);
        }

        Scope scope(const std::string &tag) const
        {
            if (!node_opt_)
                return Scope(level_, node_);

            return Scope(level_, node_, tag);
        }

        template <typename Functor>
        Scope scope(const std::string &tag, int level, Functor && functor) const
        {
            if (!operator()(level))
                //New level should not be logged
                return Scope(level_, node_);

            Scope v(level_, node_, tag);
            functor(v.node_);
            return v;
        }

        template <typename Functor>
        Scope scope(const std::string &tag, Functor && functor) const
        {
            if (!node_opt_)
                //Parent was not logged, new scope won't be logged either
                return Scope(level_, node_);

            Scope v(level_, node_, tag);
            functor(v.node_);
            return v;
        }

    private:
        static Scope *top_;

        //A higher will log more
        int level_ = -1;

        //Is only valid if this level needed actual logging
        std::optional<gubg::naft::Node> node_opt_;

        //Refers to node_opt_, or a valid node from the parent
        gubg::naft::Node &node_;

        Scope *parent_ = nullptr;
    };

} } 

#endif
