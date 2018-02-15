#ifndef HEADER_cook_model_property_KeyValue_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_property_KeyValue_hpp_ALREADY_INCLUDED

#include "cook/model/property/Interface.hpp"
#include <string>
#include <optional>

namespace cook { namespace model { namespace property {

class KeyValue : public Interface<std::string>
{
public:
    explicit KeyValue(const std::string & key)
        : Interface<std::string>(key)
    {
    }

    explicit KeyValue(const std::string & key, const std::string & value)
        : Interface<std::string>(key),
          value_(value)
    {
    }

    bool operator==(const KeyValue & rhs) const
    {
        return equal_(rhs) && value_ == rhs.value_;
    }

    bool merge(const KeyValue & rhs)
    {
        MSS_BEGIN(bool);

        MSS(merge_(*this, rhs));

        value_ = rhs.value_;

        MSS_END();
    }

    bool has_value() const                      { return value_.has_value(); }
    const std::string & value() const           { return *value_; }
    bool set_value(const std::string & value)   { value_ = value; }
    bool clear_value()                          { value_.reset(); }

private:
    std::optional<std::string> value_;
};

} } }

#endif
