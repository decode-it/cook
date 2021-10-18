#include "cook/rules/Bundle.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules { 


    Extensions BundlePList::extensions_{Language::PropertyList};
    Extensions BundleIcon::extensions_{Language::Icon};

    Bundle_family::Bundle_family(Language lang, Extensions &extensions): lang_(lang), extensions_(extensions) {}

    Language Bundle_family::language() const
    {
        return lang_;
    }

    bool Bundle_family::accepts_file(const LanguageTypePair & key, const ingredient::File & file) const
    {
        MSS_BEGIN(bool);

        if (key.language == language())
            //Language is already known and matches with ours: we accept always
            MSS_RETURN_OK();

        MSS_Q(key.language == Language::Undefined);

        //Language is not known, check if we recognise it
        MSS_Q(extensions_.is_known(file.rel().extension()));

        MSS_END();
    }

    Result Bundle_family::resolve_file(LanguageTypePair & key, ingredient::File & file) const
    {
        MSS_BEGIN(Result);

        MSS(key.language == language() || key.language == Language::Undefined);
        key.language = language();

        //Deduce the type based on the extension
        MSS(extensions_.get_type(key.type, file.rel().extension(), key.type));

        switch (key.type)
        {
            case Type::Source:
                break;
            case Type::Header:
            case Type::ForceInclude:
                file.set_propagation(Propagation::Public);
                break;

            default:
                L("Undefined type " << key.type << " for Bundle_family recipe");
                break;
        }

        file.set_overwrite(Overwrite::IfSame);

        MSS_END();
    }

    Result Bundle_family::add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const
    {
        MSS_BEGIN(Result);

        auto ss = log::scope("add c_family file", [&](auto & n) {
            n.attr("recipe", recipe.uri().string()).attr("file", file.key());
        });

        MSS(key.language == language());
        MSS(recipe.insert_or_merge(key, file));

        MSS_END();
    }


} } 
