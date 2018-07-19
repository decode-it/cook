#include "cook/rules/C_family.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules {

    Extensions CXX::extensions_{Language::CXX};
    Extensions Cc::extensions_{Language::C};
    Extensions ObjectiveCc::extensions_{Language::ObjectiveC};
    Extensions ObjectiveCXX::extensions_{Language::ObjectiveCXX};
    Extensions Resource::extensions_{Language::Resource};

    C_family::C_family(Language lang, Extensions &extensions): lang_(lang), extensions_(extensions) {}

    Language C_family::language() const
    {
        return lang_;
    }

    bool C_family::accepts_file(const LanguageTypePair & key, const ingredient::File & file) const
    {
        MSS_BEGIN(bool);

        if (key.language == language())
            //Language is already known and matches with ours: we accept always
            MSS_RETURN_OK();

        MSS_Q(key.language == Language::Undefined);

        //Language is not known, check if we recognise it
        MSS_Q(extensions_.is_known(file.key().extension()));

        MSS_END();
    }

    Result C_family::resolve_file(LanguageTypePair & key, ingredient::File & file) const
    {
        MSS_BEGIN(Result);

        MSS(key.language == language() || key.language == Language::Undefined);
        key.language = language();

        //Deduce the type based on the extension
        MSS(extensions_.get_type(key.type, file.key().extension(), key.type));

        switch (key.type)
        {
            case Type::Source:
                break;
            case Type::Header:
            case Type::ForceInclude:
                file.set_propagation(Propagation::Public);
                break;

            default:
                L("Undefined type " << key.type << " for C_family recipe");
                break;
        }

        file.set_overwrite(Overwrite::IfSame);

        MSS_END();
    }

    Result C_family::add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const
    {
        MSS_BEGIN(Result);

        auto ss = log::scope("add c_family file", [&](auto & n) {
            n.attr("recipe", recipe.uri().string()).attr("file", file.key());
        });

        MSS(key.language == language());
        MSS(recipe.files().insert_or_merge(key, file));

        MSS_END();
    }

    bool C_family::add_additional_path_(model::Recipe & recipe, const ingredient::File & file, Type type, Propagation propagation) const
    {
        MSS_BEGIN(bool);

        auto p = recipe.files().insert(LanguageTypePair(language(), type), ingredient::File(file.dir(), std::filesystem::path()));

        {
            ingredient::File & include_path = *p.first;

            include_path.set_owner(&recipe);
            include_path.set_overwrite(Overwrite::Always);
            include_path.set_propagation(propagation);
        }

        MSS_END();
    }

} }
