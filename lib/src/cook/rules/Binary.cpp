#include "cook/rules/Binary.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules {

    Extensions Binary::extensions_{Language::Binary};

Language Binary::language() const
{
    return Language::Binary;
}

bool Binary::accepts_file(const LanguageTypePair & key, const ingredient::File & file) const
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

bool Binary::resolve_file(LanguageTypePair & key, ingredient::File & file) const
{
    MSS_BEGIN(bool);

    MSS(key.language == language() || key.language == Language::Undefined);
    key.language = language();

    //Deduce the type based on the extension
    MSS(extensions_.get_type(key.type, file.key().extension(), key.type));

    switch (key.type)
    {
        case Type::Object:
            break;

        case Type::Library:
        case Type::LibraryPath:
        case Type::IncludePath:
            file.set_propagation(Propagation::Public);
            file.set_overwrite(Overwrite::Always);
            break;

        default:
            L("Undefined type " << key.type << " for Binary recipe");
            break;
    }

    MSS_END();
}

bool Binary::add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const
{
    MSS_BEGIN(bool);

    MSS(key.language == language());
    MSS(recipe.files().insert_or_merge(key, file));

    switch (key.type)
    {
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

bool Binary::add_additional_path_(model::Recipe & recipe, const ingredient::File & file, Type type, Propagation propagation) const
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
