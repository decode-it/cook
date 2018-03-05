#ifndef HEADER_cook_logger_Verbose_hpp_ALREADY_INCLUDED
#define HEADER_cook_logger_Verbose_hpp_ALREADY_INCLUDED

#include "gubg/naft/Node.hpp"
#include "gubg/debug.hpp"
#include <iostream>
#include <optional>

namespace cook { namespace logger { 

    class Verbose
    {
    public:
        Verbose(gubg::naft::Node &node): node_(node) { }
        Verbose(gubg::naft::Node &node, const std::string &tag): node_opt_{node.node(tag)}, node_(*node_opt_) { }
        Verbose(Verbose &&dying): level_(dying.level_), node_opt_(std::move(dying.node_opt_)), node_(*node_opt_) {}

        void set_level(int level) {level_ = level;}

        //Indicates if level should be logged
        bool operator()(int level) const {return level_ >= level;}

        Verbose scope(const std::string &tag, int level) const
        {
            if (!operator()(level))
            {
                Verbose v{node_};
                return v;
            }
            Verbose v{node_, tag};
            return v;
        }
        Verbose scope(const std::string &tag) const
        {
            if (!node_opt_)
            {
                Verbose v{node_};
                return v;
            }
            Verbose v{node_, tag};
            return v;
        }

        template <typename Key, typename Value>
        Verbose &attr(const Key &key, const Value &value)
        {
            if (!!node_opt_)
                node_opt_->attr(key, value);
            return *this;
        }
        template <typename Key>
        Verbose &attr(const Key &key)
        {
            if (!!node_opt_)
                node_opt_->attr(key);
            return *this;
        }
        template <typename Key, typename Value>
        Verbose &attr(const std::pair<Key, Value> &pair)
        {
            return attr(pair.first, pair.second);
        }

    private:
        //A higher will log more
        int level_ = -1;

        //Is only valid if this level needed actual logging
        std::optional<gubg::naft::Node> node_opt_;

        //Refers to node_opt_, or a valid node from the parent
        gubg::naft::Node &node_;
    };

} } 

#endif
