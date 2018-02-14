#ifndef HEADER_cook_model_property_Properties_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_property_Properties_hpp_ALREADY_INCLUDED

#include "cook/model/property/Collection.hpp"
#include "cook/Language.hpp"
#include "cook/Type.hpp"
#include <map>

namespace cook { namespace model { namespace property {

struct PropertiesKey
{
    PropertiesKey()
        : language(Language::Undefined), type(Type::Undefined)
    {
    }

    PropertiesKey(Language language, Type type)
        : language(language), type(type)
    {
    }


    Language language;
    Type type;
};

bool operator<(const PropertiesKey & lhs, const PropertiesKey & rhs)
{
    return lhs.language < rhs.language || (lhs.language == rhs.language && lhs.type < rhs.type);
}

template <typename Property>
struct Properties
{
    bool add(const PropertiesKey & key, const Property & property)
    {
        MSS_BEGIN(bool);

        // add if necessary
        Collection<Property> & collection = properties_[key];
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
    std::map<PropertiesKey, Collection<Property>> properties_;
};

} } }

#endif
