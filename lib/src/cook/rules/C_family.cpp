#include "cook/rules/C_family.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules {

    Extensions CXX::extensions_{Language::CXX};
    Extensions Cc::extensions_{Language::C};

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

        //Language is not known, check if the file exists and we recognise it
        {
            const std::filesystem::path & fn = file.key();
            MSS_Q(std::filesystem::is_regular_file(fn));
            MSS_Q(extensions_.is_known(fn.extension()));
        }

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
            case Type::Header:
            case Type::ForceInclude:
                break;

            default:
                L("Undefined type " << key.type << " for C_family recipe");
                break;
        }

        MSS_END();
    }

    Result C_family::add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const
    {
        MSS_BEGIN(Result);

        auto ss = log::scope("add file", [&](auto & n) {
            n.attr("recipe", recipe.uri().string()).attr("file", file.key());
        });

        MSS(key.language == language());
        MSS(recipe.files().insert_or_merge(key, file));

        switch (key.type)
        {
            case Type::Header:
            case Type::ForceInclude:
                MSS(add_additional_path_(recipe, file, Type::IncludePath, Propagation::Public));
                break;

            case Type::Object:
            case Type::Library:
                // add also the dir part with same propagation as the file itself
                MSS(add_additional_path_(recipe, file, Type::LibraryPath, file.propagation()));
                break;

            default:
                break;
        }

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
