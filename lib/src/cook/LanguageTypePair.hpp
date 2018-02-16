#ifndef HEADER_cook_LanguageTypePair_hpp_ALREADY_INCLUDED
#define HEADER_cook_LanguageTypePair_hpp_ALREADY_INCLUDED

#include "cook/Language.hpp"
#include "cook/Type.hpp"
#include <utility>
#include <ostream>

namespace cook {

struct LanguageTypePair
{
    LanguageTypePair() {}

    LanguageTypePair(Language language, Type type)
        : language(language),
          type(type)
    {
    }

    Language language = Language::Undefined;
    Type type = Type::Undefined;

    bool operator<(const LanguageTypePair & rhs)  const { return to_pair() <  rhs.to_pair(); }
    bool operator==(const LanguageTypePair & rhs) const { return to_pair() == rhs.to_pair(); }

    std::pair<Language, Type> to_pair() const { return std::make_pair(language, type); }
};

inline std::ostream &operator<<(std::ostream &os, const LanguageTypePair &pair)
{
    os << "(" << pair.language << ", " << pair.type << ")";
    return os;
}

}

#endif
