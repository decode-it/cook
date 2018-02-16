#ifndef HEADER_cook_LanguageTypePair_hpp_ALREADY_INCLUDED
#define HEADER_cook_LanguageTypePair_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include "cook/Type.hpp"

namespace cook {

struct LanguageTypePair
{
    LanguageTypePair()
        : language(Language::Undefined),
          type(Type::Undefined)
    {
    }

    LanguageTypePair(Language language, Type type)
        : language(language),
          type(type)
    {
    }

    Language language;
    Type type;

    bool operator<(const LanguageTypePair & rhs) const { return language < rhs.language || (language == rhs.language && type < rhs.type); }
    bool operator==(const LanguageTypePair & rhs) const { return language == rhs.language && type == rhs.type; }
};

}

#endif
