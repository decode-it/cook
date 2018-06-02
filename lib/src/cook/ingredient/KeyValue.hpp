#ifndef HEADER_cook_ingredient_KeyValue_hpp_ALREADY_INCLUDED
#define HEADER_cook_ingredient_KeyValue_hpp_ALREADY_INCLUDED

#include "cook/ingredient/Base.hpp"
#include "cook/Log.hpp"
#include <string>
#include <optional>

namespace cook { namespace ingredient {

    class KeyValue : public Base<std::string>
    {
    public:
        explicit KeyValue(const std::string & key)
        : Base<std::string>(key)
        {
        }

        explicit KeyValue(const std::string & key, const std::string & value)
        : Base<std::string>(key),
        value_(value)
        {
        }

        bool operator==(const KeyValue & rhs) const
        {
            return equal_(rhs) && value_ == rhs.value_;
        }

        Result merge(const KeyValue & rhs)
        {
            MSS_BEGIN(Result);

            auto ss = log::scope("merge ingredient");
            {
                auto s = log::scope("source");
                rhs.stream();
            }
            {
                auto s = log::scope("target");
                stream();
            }


            MSS(merge_(*this, rhs));

            value_ = rhs.value_;

            MSS_END();
        }

        bool has_value() const                      { return value_.has_value(); }
        const std::string & value() const           { return *value_; }
        void set_value(const std::string & value)   { value_ = value; }
        void clear_value()                          { value_.reset(); }
        std::string to_string() const
        {
            if (!value_)
                return key();
            else
                return key() + "=" + value();
        }

        log::Scope stream(log::Importance importance = log::Importance()) const
        {
            const auto imp = log::importance(importance);

            return log::scope("key_value", imp, [&](auto &n) {
                    n.attr("key", key());
                    if (has_value())
                    n.attr("value", value());
                    n.attr("prop", propagation());
                    });
        }


    private:
        std::optional<std::string> value_;
    };

    inline std::ostream &operator<<(std::ostream &os, const KeyValue & key_value)
    {
        os << "Key" <<  key_value.key() << " |";

        if (key_value.has_value())
            os << " " << key_value.value();

        return os;
    }

} }

#endif
