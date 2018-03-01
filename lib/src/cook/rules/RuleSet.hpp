#ifndef HEADER_cook_rules_RuleSet_hpp_ALREADY_INCLUDED
#define HEADER_cook_rules_RuleSet_hpp_ALREADY_INCLUDED

#include "cook/rules/Interface.hpp"
#include <memory>
#include <map>
#include <cassert>

namespace cook { namespace rules {

class RuleSet
{

public:
    using Ptr = std::shared_ptr<RuleSet>;

    RuleSet();

    static Ptr create(){return Ptr(new RuleSet);}

    template <typename Functor>
    const Interface & find(Functor && functor) const
    {
        for(auto it = priority_map_.begin(); it != priority_map_.end(); ++it)
        {
            const Interface & interface = (*it->second.get());

            if (functor(interface))
                return interface;
        }

        return *default_interface_;
    }

private:
    std::multimap<unsigned int, Interface::Ptr> priority_map_;
    Interface::Ptr default_interface_;
};

} }

#endif
