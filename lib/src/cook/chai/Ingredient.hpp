#ifndef HEADER_cook_chai_Ingredient_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Ingredient_hpp_ALREADY_INCLUDED

#include "cook/chai/Recipe.hpp"
#include "cook/chai/Flags.hpp"
#include "cook/ingredient/Base.hpp"
#include "cook/LanguageTypePair.hpp"

namespace cook { namespace chai {

template <typename T> class Ingredient
{
public:
    Ingredient(const LanguageTypePair & language_type_pair, const T & element, const Context * context)
        : flags_(Flags(language_type_pair.language) | Flags(language_type_pair.type) | Flags(element.overwrite()) | Flags(element.propagation())),
          context_(context),
          element_(element)
    {}

    bool has_owner() const      { return element_.owner(); }
    Recipe owner() const        { return Recipe(element_.owner(), context_); }
    const Flags & flags() const { return flags_; }
    Flags &  flags()            { return flags_; }

    T & element()                                           { return element_; }
    const T & element() const                               { return element_; }
    LanguageTypePair language_type_pair() const             { return LanguageTypePair(flags_.get_or(Language::Undefined), flags_.get_or(Type::Undefined)); }


    void install_flags()
    {
        element_.set_propagation(flags_.get_or(element_.propagation()));
        element_.set_overwrite(flags_.get_or(element_.overwrite()));
    }

private:
    Flags flags_;
    const Context * context_;
    T element_;
};

} }

#endif
