#ifndef HEADER_cook_rules_Binary_hpp_ALREADY_INCLUDED
#define HEADER_cook_rules_Binary_hpp_ALREADY_INCLUDED

#include "cook/rules/Interface.hpp"
#include "cook/rules/Extensions.hpp"
#include "cook/LanguageTypePair.hpp"
#include "cook/ingredient/File.hpp"
#include <set>

namespace cook { namespace rules {

class Binary : public Interface
{
public:
    Language language() const override;
    bool accepts_file(const LanguageTypePair & key, const ingredient::File & file) const override;
    Result resolve_file(LanguageTypePair &key, ingredient::File &file) const override;
    Result add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const override;

private:
    bool add_additional_path_(model::Recipe & recipe, const ingredient::File & file, Type type, Propagation propagation) const;

    static Extensions extensions_;
};

} }

#endif
