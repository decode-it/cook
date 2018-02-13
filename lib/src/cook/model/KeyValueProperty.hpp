#ifndef HEADER_cook_model_KeyValueProperty_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_KeyValueProperty_hpp_ALREADY_INCLUDED

#include "cook/model/Property.hpp"
#include <string>
#include <optional>

namespace cook { namespace model {

class KeyValueProperty : public Property<std::string>
{
public:
    explicit KeyValueProperty(const std::string & key, Recipe * owner)
        : Property<std::string>(key, owner)
    {
    }

    explicit KeyValueProperty(const std::string & key, const std::string & value, Recipe * owner)
        : Property<std::string>(key, owner),
          value_(value)
    {
    }

    bool operator==(const KeyValueProperty & rhs) const
    {
        return equal_(rhs) && value_ == rhs.value_;
    }

    bool has_value() const                      { return value_; }
    const std::string & value() const           { return *value_; }
    bool set_value(const std::string & value)   { value_ = value; }
    bool clear_value()                          { value_.reset(); }

private:
    std::optional<std::string> value_;
};

} }

#endif
