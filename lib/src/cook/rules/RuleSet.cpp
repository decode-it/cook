#include "cook/rules/RuleSet.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules {

namespace  {

class DefaultSink : public Interface
{
    Language language() const override
    {
        return Language::Undefined;
    }
    bool accepts_file(const LanguageTypePair & key, const ingredient::File & file) const override
    {
        return true;
    }
    bool resolve_file(LanguageTypePair & key, ingredient::File & file) const override
    {
        return true;
    }
    bool add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const
    {
        MSS_BEGIN(bool);
        MSS(recipe.files().insert_or_merge(key, file));
        MSS_END();
    }
};

}

RuleSet::RuleSet()
    : default_interface_(std::make_shared<DefaultSink>())
{
}

} }
