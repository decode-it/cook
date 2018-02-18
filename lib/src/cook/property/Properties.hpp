#ifndef HEADER_cook_property_Properties_hpp_ALREADY_INCLUDED
#define HEADER_cook_property_Properties_hpp_ALREADY_INCLUDED

#include "cook/property/Collection.hpp"
#include "cook/LanguageTypePair.hpp"
#include <map>

namespace cook { namespace property {

template <typename Interface> class Properties
{
    using PropertyMap = std::map<LanguageTypePair, Collection<Interface>>;

public:
    using iterator = typename PropertyMap::iterator;
    using const_iterator = typename PropertyMap::const_iterator;

    bool insert_or_merge(const LanguageTypePair & key, const Interface & property)
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

    Collection<Interface> & operator[](const LanguageTypePair & key)
    {
        return properties_[key];
    }

    iterator begin()                { return properties_.begin(); }
    iterator end()                  { return properties_.end(); }
    const_iterator begin() const    { return properties_.begin(); }
    const_iterator end() const      { return properties_.end(); }

    iterator find(const LanguageTypePair & collection_key)              { return properties_.find(collection_key); }
    const_iterator find(const LanguageTypePair & collection_key) const  { return properties_.find(collection_key); }

    Interface * find(const LanguageTypePair & collection_key, const typename Interface::key_type & property_key)
    {
        return find_<Interface>(collection_key, property_key, properties_);
    }

    const Interface * find(const LanguageTypePair & collection_key, const typename Interface::key_type & property_key) const
    {
        return find_<const Interface>(collection_key, property_key, properties_);
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

    PropertyMap properties_;
};

} }

#endif
