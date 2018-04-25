#include "cook/chai/KeyValue.hpp"

namespace cook { namespace chai {

KeyValue::KeyValue(const LanguageTypePair & ltp, const ingredient::KeyValue & key_value, const Context *context)
    : Ingredient<ingredient::KeyValue>(ltp, key_value, context)
{
}

const std::string & KeyValue::key() const
{
    return element().key();
}

bool KeyValue::has_value() const
{
    return element().has_value();
}
const std::string & KeyValue::value() const
{
    return element().value();
}


} }

