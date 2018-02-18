#ifndef HEADER_cook_property_Properties_hpp_ALREADY_INCLUDED
#define HEADER_cook_property_Properties_hpp_ALREADY_INCLUDED

#include "cook/property/Collection.hpp"
#include "cook/LanguageTypePair.hpp"
#include <map>

namespace cook { namespace property {

template <typename Property>
struct Properties
{
    bool add_or_merge(const LanguageTypePair & key, const Property & property)
    {
        MSS_BEGIN(bool);

        auto p = insert(key, property);
        if (!p.second)
            MSS(p.first->merge(property));

        MSS_END();
    }

    std::pair<typename Collection<Property>::iterator, bool> insert(const LanguageTypePair & collection_key, const Property & property)
    {
        // add if necessary
        Collection<Property> & collection = properties_[collection_key];
        return collection.insert(property);
    }


    Property * find(const LanguageTypePair & collection_key, const typename Property::key_type & property_key)
    {
        return find_<Property>(collection_key, property_key, properties_);
    }

    const Property * find(const LanguageTypePair & collection_key, const typename Property::key_type & property_key) const
    {
        return find_<const Property>(collection_key, property_key, properties_);
    }

    template <typename Functor>
    bool each(Functor && functor) const
    {
        for(const auto & p : properties_)
            for(const auto & file : p.second)
                if (!functor(p.first, file))
                    return false;

        return true;
    }


private:
    template <typename Property_, typename Properties_>
    Property_ * find_(const LanguageTypePair & collection_key, const typename Property::key_type & property_key, Properties_ & properties) const
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

    std::map<LanguageTypePair, Collection<Property>> properties_;
};

} }

#endif
