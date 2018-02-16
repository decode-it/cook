#ifndef HEADER_cook_property_Properties_hpp_ALREADY_INCLUDED
#define HEADER_cook_property_Properties_hpp_ALREADY_INCLUDED

#include "cook/property/Collection.hpp"
#include "cook/LanguageTypePair.hpp"
#include <map>

namespace cook { namespace property {

template <typename Interface>
struct Properties
{
    bool add(const LanguageTypePair & key, const Interface & property)
    {
        MSS_BEGIN(bool);

        // add if necessary
        Collection<Interface> & collection = properties_[key];
        auto it = collection.find(property.key());

        // new, just add
        if (it == collection.end())
            collection.insert(property);
        // otherwise try to merge
        else
            MSS(it->merge(property));

        MSS_END();
    }


private:
    std::map<LanguageTypePair, Collection<Interface>> properties_;
};

} }

#endif
