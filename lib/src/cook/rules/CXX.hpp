#ifndef HEADER_cook_rules_CXX_hpp_ALREADY_INCLUDED
#define HEADER_cook_rules_CXX_hpp_ALREADY_INCLUDED

#include "cook/rules/Interface.hpp"
#include "cook/LanguageTypePair.hpp"
#include "cook/property/File.hpp"
#include <set>

namespace cook { namespace rules {

class CXX : public Interface
{
public:
    Language language() const override;
    bool accepts(const LanguageTypePair & key, const property::File & file) const override;


    static std::set<std::string> extensions_;
};

} }

#endif
