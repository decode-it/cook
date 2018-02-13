#ifndef HEADER_cook_model_PropertySet_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_PropertySet_hpp_ALREADY_INCLUDED

#include "cook/model/Property.hpp"
#include "cook/Type.hpp"
#include "cook/Language.hpp"
#include "gubg/Range.hpp"
#include <vector>
#include <algorithm>

namespace cook { namespace model {

template <typename Property>
class PropertySet
{
    using PropertyContainer = std::vector<Property>;

public:
    using key_type = typename Property::key_type;
    using const_iterator = typename PropertyContainer::const_iterator;

    explicit PropertySet(Language language, Type type)
        : language_(language),
          type_(type)
    {
    }

    Language language() const   { return language_; }
    Type type() const           { return type_; }

    std::pair<const_iterator, bool> insert(const Property & property)
    {
        return insert_(property);
    }
    std::pair<const_iterator, bool> insert(Property && property)
    {
        return insert_(std::move(property));
    }

    const_iterator find(const key_type & key) const
    {
        return find_(gubg::make_range(properties_), key);
    }
    const_iterator begin() const { return properties_.begin(); }
    const_iterator end() const { return properties_.end(); }

    std::size_t size() const { return properties_.size(); }

private:
    template <typename It>
    static It find_(gubg::Range<It> range, const key_type & key)
    {
        return std::find_if(range.begin(), range.end(), [&](const auto & el) { return el.key() == key; });
    }

    template <typename AProperty>
    std::pair<const_iterator, bool> insert_(AProperty && property)
    {
        const_iterator it = find_(gubg::make_range(properties_), property.key());

        // already present ?
        if (it != end())
            return std::make_pair(it, false);

        // nope, insert it
        properties_.push_back(std::forward<AProperty>(property));
        it = (properties_.end() - 1);
        return std::make_pair(it, true);
    }

    std::vector<Property> properties_;
    Language language_;
    Type type_;
};

} }

#endif
