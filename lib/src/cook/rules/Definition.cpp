#include "cook/rules/Definition.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules { 

    Extensions Definition::extensions_{Language::Definition};

    Language Definition::language() const
    {
        return Language::Definition;
    }

    bool Definition::accepts_file(const LanguageTypePair & key, const ingredient::File & file) const
    {
        MSS_BEGIN(bool);

        if (key.language == language())
            //Language is already known and matches with ours: we accept always
            MSS_RETURN_OK();

        MSS_Q(key.language == Language::Undefined);

        //Language is not known, check if the file exists and we recognise it
        MSS_Q(extensions_.is_known(file.rel().extension()));

        MSS_END();
    }

    Result Definition::resolve_file(LanguageTypePair & key, ingredient::File & file) const
    {
        MSS_BEGIN(Result);

        MSS(key.language == language() || key.language == Language::Undefined);
        key.language = language();

        //Deduce the type based on the extension
        MSS(extensions_.get_type(key.type, file.rel().extension(), key.type));

        switch (key.type)
        {
            case Type::Export:
                break;

            default:
                L("Undefined type " << key.type << " for Definition recipe");
                break;
        }

        file.set_overwrite(Overwrite::IfSame);
        file.set_propagation(Propagation::Public);

        MSS_END();
    }

    Result Definition::add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const
    {
        MSS_BEGIN(Result);

        auto ss = log::scope("add assembler file", [&](auto & n) {
            n.attr("recipe", recipe.uri().string()).attr("file", file.key());
        });

        MSS(key.language == language());
        MSS(recipe.insert_or_merge(key, file));

        MSS_END();
    }

} } 
