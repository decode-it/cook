#ifndef HEADER_cook_model_object_Collection_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_object_Collection_hpp_ALREADY_INCLUDED

#include "cook/model/object/Object.hpp"
#include <map>
#include <vector>
#include <algorithm>

namespace cook { namespace model { namespace object {

template <typename T> class Collection
{
    using ObjectList = std::vector<T>;

public:
    const T * get(Type type, const std::string & key) const
    {
        auto lst_it = elements_.find(type);
        if (lst_it == elements_.end())
            return nullptr;

        const ObjectList & lst = lst_it->second;

        auto el_it = std::find_if(lst.begin(), lst.end(), [&](const T & el) { return el.key() == key; });

        if (el_it == lst.end())
            return nullptr;
        else
            return &*el_it;
    }

    T * get(Type type, const std::string & key)
    {
        auto lst_it = elements_.find(type);
        if (lst_it == elements_.end())
            return nullptr;

        ObjectList & lst = lst_it->second;

        auto el_it = std::find_if(lst.begin(), lst.end(), [&](const T & el) { return el.key() == key; });

        if (el_it == lst.end())
            return nullptr;
        else
            return &*el_it;
    }

    template <typename Functor> void each(Type type, Functor && f) const
    {
        auto lst_it = elements_.find(type);
        if(lst_it == elements_.end())
            return;

        const ObjectList & lst = lst_it->second;
        for(const T & p : lst)
            f(p);
    }

    template <typename Functor>
    void each(std::initializer_list<Type> types, Functor && f) const
    {
        for(auto t : types)
        {
            auto lst_it = elements_.find(t);
            if(lst_it == elements_.end())
                continue;

            const ObjectList & lst = lst_it->second;
            for(const T & p : lst)
                f(t, p);
        }
    }

    bool insert(const T & element, cook_Overwrite_t overwrite_reason)
    {
        return insert(element.type(), element, overwrite_reason);
    }

    bool insert(Type type, const T & element, cook_Overwrite_t overwrite_reason)
    {
        ObjectList & lst = elements_[type];

        // check for the same element in the list with this key
        auto it = std::find_if(lst.begin(), lst.end(), [&](const T & el) { return element.key() == el.key(); });

        // an element with this key already exists
        if (it != lst.end())
        {
            bool is_okay = false;

            // can we overwrite it?
            switch(overwrite_reason)
            {
                case cook_Overwrite_Always: is_okay = true; break;
                case cook_Overwrite_IfSame: is_okay = element == *it; break;
            }

            // no, so give a false message
            if (!is_okay)
                return false;

            // yes, remove it from the list
            lst.erase(it);
        }

        // add to the end of the list
        lst.push_back(element);

        return true;
    }


private:
    std::map<Type, ObjectList> elements_;
};

} } }

#endif
