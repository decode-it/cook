#ifndef HEADER_cook_chai_KeyValue_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_KeyValue_hpp_ALREADY_INCLUDED

#include "cook/chai/Ingredient.hpp"
#include "cook/ingredient/KeyValue.hpp"

namespace cook { namespace chai {

class KeyValue : public Ingredient<ingredient::KeyValue>
{
public:
    KeyValue(const LanguageTypePair & ltp, const ingredient::KeyValue & key_value, const Context * context);

    bool is_file() const        { return true; }
    bool is_key_value() const   { return false; }

    const std::string & key() const;
    const std::string & name() const;
    bool has_value() const;
    const std::string & value() const;
};

} }



#endif // KEYVALUE_HPP
