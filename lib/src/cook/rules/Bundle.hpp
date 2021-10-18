#ifndef HEADER_cook_rules_Bundle_hpp_ALREADY_INCLUDED
#define HEADER_cook_rules_Bundle_hpp_ALREADY_INCLUDED

#include "cook/rules/Interface.hpp"
#include "cook/rules/Extensions.hpp"
#include "cook/LanguageTypePair.hpp"
#include "cook/ingredient/File.hpp"

namespace cook { namespace rules { 

    class Bundle_family : public Interface
    {
    public:
        Bundle_family(Language lang, Extensions &extensions);

        Language language() const override;
        bool accepts_file(const LanguageTypePair & key, const ingredient::File & file) const override;
        Result resolve_file(LanguageTypePair &key, ingredient::File &file) const override;
        Result add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const override;

    private:
        bool add_additional_path_(model::Recipe & recipe, const ingredient::File & file, Type type, Propagation propagation) const;

        const Language lang_;
        Extensions &extensions_;
    };

    class BundlePList: public Bundle_family
    {
    public:
        BundlePList(): Bundle_family(Language::PropertyList, extensions_){}

    private:
        static Extensions extensions_;
    };

    class BundleIcon: public Bundle_family
    {
    public:
        BundleIcon(): Bundle_family(Language::Icon, extensions_){}

    private:
        static Extensions extensions_;
    };

} } 

#endif
