#ifndef HEADER_cook_message_Property_hpp_ALREADY_INCLUDED
#define HEADER_cook_message_Property_hpp_ALREADY_INCLUDED

#include "cook/Message.hpp"
#include "cook/Overwrite.hpp"

namespace cook { namespace message {

template <typename Key>
Message error_property_keys_differ(const Key & k1, const Key & k2)
{
    return make_internal_error_message([=](auto & os) {os << "Supplied keys differ: " << k1 << " != " << k2; });
}

template <typename Key>
Message info_property_refining_overwrite(const Key & key, Overwrite current)
{
    return make_info_message([=](auto & os) {os << "Refining overwrite policy for " << key << " to " << current; });
}

template <typename Key>
Message error_property_no_overwrite_allows(const Key & key, Overwrite overwrite)
{
    return make_error_message([=](auto & os) { os << "No overwrite allows for " << key << " (" << overwrite << ")"; });
}

} }

#endif
