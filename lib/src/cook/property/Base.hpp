#ifndef HEADER_cook_property_Base_hpp_ALREADY_INCLUDED
#define HEADER_cook_property_Base_hpp_ALREADY_INCLUDED

#include "cook/Propagation.hpp"
#include "cook/Overwrite.hpp"
#include "cook/Result.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace model {

class Recipe;

}

namespace property {


template <typename KeyType>
class Base
{
public:
    using key_type = KeyType;

    explicit Base(const KeyType & key)
        : key_(key),
          propagation_(Propagation::Private),
          owner_(nullptr),
          overwrite_(Overwrite::Never)
    {
    }

    const KeyType & key() const                     { return key_; }
    model::Recipe * owner() const                   { return owner_; }
    Propagation propagation() const                 { return propagation_; }
    Overwrite overwrite() const                     { return overwrite_; }
    void set_propagation(Propagation propagation)   { propagation_ = propagation; }
    void set_overwrite(Overwrite overwrite)         { overwrite_ = overwrite; }
    void set_owner(model::Recipe * owner)           { owner_ = owner; }

protected:
    bool equal_(const Base<KeyType> & rhs) const
    {
        return key_ == rhs.key_
                && propagation_ == rhs.propagation_
                && owner_ == rhs.owner_;
    }

    template <typename Property>
    static bool merge_(Property & lhs, const Property & rhs)
    {
        MSS_BEGIN(bool);
        MSS(lhs.key() == rhs.key());

        // should we update the overwrite policy ?
        Overwrite ov = lhs.overwrite();
        if (ov != rhs.overwrite())
            ov = std::min(ov, rhs.overwrite());

        switch(ov)
        {
            case Overwrite::Always:
                break;

            case Overwrite::IfSame:
                MSS(lhs == rhs);
                break;

            default:
                MSS(false);
                break;
        }

        lhs.overwrite_ = ov;
        lhs.propagation_ = rhs.propagation();
        lhs.owner_ = rhs.owner();

        MSS_END();
    }

private:
    KeyType key_;
    Propagation propagation_;
    model::Recipe * owner_;
    Overwrite overwrite_;
};

} }

#endif
