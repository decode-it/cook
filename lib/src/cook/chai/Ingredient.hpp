#ifndef HEADER_cook_chai_Ingredient_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Ingredient_hpp_ALREADY_INCLUDED

#include "cook/chai/Recipe.hpp"
#include "cook/chai/Flags.hpp"
#include "cook/ingredient/Base.hpp"
#include "cook/LanguageTypePair.hpp"
#include <boost/variant.hpp>

namespace cook { namespace chai {

template <typename T> class Ingredient
{
public:
    Ingredient(const LanguageTypePair & language_type_pair, const T & element) : ltp_(language_type_pair), element_(element) {}

    bool has_owner() const { return element_.owner(); }
    Recipe owner() const    { return Recipe(element_.owner()); }
    Flags flags() const { return Flags(ltp_.language) | Flags(ltp_.type) | Flags(element_.overwrite()) | Flags(element_.propagation()); }

    T & element()                                           { return element_; }
    const T & element() const                               { return element_; }
    LanguageTypePair & language_type_pair()                 { return ltp_; }
    const LanguageTypePair & language_type_pair() const     { return ltp_; }

private:
    LanguageTypePair ltp_;
    T element_;
};

} }

#endif
