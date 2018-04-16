#include "cook/rules/ASM.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules {

    Extensions ASM::extensions_{Language::ASM};

Language ASM::language() const
{
    return Language::ASM;
}

bool ASM::accepts_file(const LanguageTypePair & key, const ingredient::File & file) const
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

Result ASM::resolve_file(LanguageTypePair & key, ingredient::File & file) const
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

        default:
            L("Undefined type " << key.type << " for ASM recipe");
            break;
    }

    MSS_END();
}

Result ASM::add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const
{
    MSS_BEGIN(Result);

    MSS(key.language == language());
    MSS(recipe.files().insert_or_merge(key, file));

    MSS_END();
}

} }
