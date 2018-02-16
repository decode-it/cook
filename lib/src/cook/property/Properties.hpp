#ifndef HEADER_cook_property_Properties_hpp_ALREADY_INCLUDED
#define HEADER_cook_property_Properties_hpp_ALREADY_INCLUDED

#include "cook/property/Collection.hpp"
#include "cook/LanguageTypePair.hpp"
#include <map>

namespace cook { namespace property {

template <typename Interface>
struct Properties
{
    bool add_or_merge(const LanguageTypePair & key, const Interface & property)
    {
        MSS_BEGIN(bool);

        auto p = insert(key, property);
        if (!p.second)
            MSS(p.first->merge(property));

        MSS_END();
    }

    std::pair<typename Collection<Interface>::iterator, bool> insert(const LanguageTypePair & collection_key, const Interface & property)
    {
        // add if necessary
        Collection<Interface> & collection = properties_[collection_key];
        return collection.insert(property);
    }


    Interface * find(const LanguageTypePair & collection_key, const typename Interface::key_type & property_key)
    {
        return find_<Interface>(collection_key, property_key, properties_);
    }

    const Interface * find(const LanguageTypePair & collection_key, const typename Interface::key_type & property_key) const
    {
        return find_<const Interface>(collection_key, property_key, properties_);
    }


private:



    template <typename Interface_, typename Properties_>
    Interface_ * find_(const LanguageTypePair & collection_key, const typename Interface::key_type & property_key, Properties_ & properties) const
    {
        auto pit = properties.find(collection_key);
        if (pit == properties.end())
            return nullptr;


        auto & collection = pit->second;
        auto cit = collection.find(property_key);

        if (cit == collection.end())
            return nullptr;

        return &*cit;
    }

    std::map<LanguageTypePair, Collection<Interface>> properties_;
};

} }

#endif
