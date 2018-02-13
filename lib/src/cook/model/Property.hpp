#ifndef HEADER_cook_model_Property_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Property_hpp_ALREADY_INCLUDED

#include "cook/Propagation.hpp"

namespace cook { namespace model {

class Recipe;

template <typename KeyType>
class Property
{
public:
    using key_type = KeyType;

    explicit Property(const KeyType & key, Recipe * owner)
        : key_(key),
          owner_(owner)
    {
    }

    const KeyType & key() const                           { return key_; }
    Recipe * owner() const                          { return owner_; }
    Propagation propagation() const                 { return propagation_; }
    void set_propagation(Propagation propagation)   { propagation_ = propagation; }

protected:
    bool equal_(const Property<KeyType> & rhs) const
    {
        return key_ == rhs.key_
                && propagation_ == rhs.propagation_
                && owner_ == rhs.owner_;
    }

private:
    KeyType key_;
    Propagation propagation_;
    Recipe * owner_;
};

} }

#endif
