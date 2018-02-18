#ifndef HEADER_cook_property_Collection_hpp_ALREADY_INCLUDED
#define HEADER_cook_property_Collection_hpp_ALREADY_INCLUDED

#include "cook/property/Interface.hpp"
#include "gubg/Range.hpp"
#include <vector>
#include <algorithm>

namespace cook { namespace property {

template <typename Property>
class Collection
{
    using Container = std::vector<Property>;

public:
    using key_type = typename Property::key_type;
    using const_iterator = typename Container::const_iterator;
    using iterator = typename Container::iterator;

    void clear() { properties_.clear(); }

    std::pair<iterator, bool> insert(const Property & property) { return insert_(property); }
    std::pair<iterator, bool> insert(Property && property)      { return insert_(std::move(property)); }

    const_iterator find(const key_type & key) const { return find_(gubg::make_range(properties_), key); }
    iterator find(const key_type & key)             { return find_(gubg::make_range(properties_), key); }

    const_iterator begin() const    { return properties_.begin(); }
    const_iterator end() const      { return properties_.end(); }
    iterator begin()                { return properties_.begin(); }
    iterator end()                  { return properties_.end(); }

    iterator erase(iterator position)               { return properties_.erase(position); }
    std::size_t erase(const key_type & key)
    {
        auto it = std::remove_if(properties_.begin(), properties_.end(), [&](const auto & el) { return el.key() == key; });
        std::size_t removed = std::distance(it, properties_.end());
        properties_.erase(it, properties_.end());
        return removed;
    }

    std::size_t size() const { return properties_.size(); }

private:
    template <typename It>
    static It find_(gubg::Range<It> range, const key_type & key)
    {
        return std::find_if(range.begin(), range.end(), [&](const auto & el) { return el.key() == key; });
    }

    template <typename AProperty>
    std::pair<iterator, bool> insert_(AProperty && property)
    {
        iterator it = find_(gubg::make_range(properties_), property.key());

        // already present ?
        if (it != end())
            return std::make_pair(it, false);

        // nope, insert it
        properties_.push_back(std::forward<AProperty>(property));
        it = (properties_.end() - 1);
        return std::make_pair(it, true);
    }

    std::vector<Property> properties_;
};


} }

#endif
