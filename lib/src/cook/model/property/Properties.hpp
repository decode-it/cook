#ifndef HEADER_cook_model_property_Properties_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_property_Properties_hpp_ALREADY_INCLUDED

#include "cook/model/property/Collection.hpp"
#include "cook/model/property/PropertiesKey.hpp"
#include <map>

namespace cook { namespace model { namespace property {

template <typename Interface>
struct Properties
{
    bool add(const PropertiesKey & key, const Interface & property)
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
    std::map<PropertiesKey, Collection<Interface>> properties_;
};

} } }

#endif
