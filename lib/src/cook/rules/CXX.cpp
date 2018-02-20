#include "cook/rules/CXX.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules {

std::set<std::string> CXX::source_extensions_   = { ".cpp", ".cxx", ".CPP", ".CXX" };
std::set<std::string> CXX::header_extensions_   = { ".hpp", ".hxx", ".HPP", ".HXX" };
std::set<std::string> CXX::object_extensions_   = { ".o", ".obj" };
std::set<std::string> CXX::lib_extensions_      = { ".a", ".so", ".lib", ".dll" };

Language CXX::language() const
{
    return Language::CXX;
}

bool CXX::accepts_file(const LanguageTypePair & key, const ingredient::File & file) const
{
    MSS_BEGIN(bool);

    if (key.language == language())
        //Language is already known and matches with ours: we accept always
        MSS_RETURN_OK();
    
    MSS_Q(key.language == Language::Undefined);

    //Language is not known, check if the file exists and we recognise it
    {
        const std::filesystem::path & fn = file.key();
        MSS(std::filesystem::is_regular_file(fn));
        MSS(type_from_extension(fn.extension()));
    }

    MSS_END();
}

bool CXX::resolve_file(LanguageTypePair & key, ingredient::File & file) const
{
    MSS_BEGIN(bool);

    MSS(key.language == language() || key.language == Language::Undefined);
    key.language = language();

    //Deduce the type based on the extension
    MSS(type_from_extension(key.type, file.key().extension(), key.type));

    switch (key.type)
    {
        case Type::Source:
        case Type::Header:
        case Type::ForceInclude:
        case Type::Object:
            break;

        case Type::Library:
        case Type::LibraryPath:
        case Type::IncludePath:
            file.set_propagation(Propagation::Public);
            file.set_overwrite(Overwrite::Always);
            break;

        default:
            L("Undefined type " << key.type << " for CXX recipe");
            break;
    }

    MSS_END();
}

bool CXX::add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const
{
    MSS_BEGIN(bool);

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

bool CXX::add_additional_path_(model::Recipe & recipe, const ingredient::File & file, Type type, Propagation propagation) const
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

bool CXX::type_from_extension(Type &dst, const std::string & extension, Type src)
{
    MSS_BEGIN(bool);

    if (src == Type::Undefined)
    {
        //The type is not known yet: try to resolve it from the extension
        if (false) {}
        else if (source_extensions_.find(extension) != source_extensions_.end())
            src = Type::Source;
        else if (header_extensions_.find(extension) != header_extensions_.end())
            src = Type::Header;
        else if (object_extensions_.find(extension) != object_extensions_.end())
            src = Type::Object;
        else if (lib_extensions_.find(extension) != lib_extensions_.end())
            src = Type::Library;

        MSS_Q(src != Type::Undefined);
    }

    dst = src;

    MSS_END();
}

} }
