#include "cook/rules/CXX.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules {

namespace {

static Language cxx() { return Language::CXX; }

}

std::set<std::string> CXX::source_extensions_ = { ".cpp", ".cxx", ".CPP", ".CXX" };
std::set<std::string> CXX::header_extensions_ = { ".hpp", ".hxx", ".HPP", ".HXX" };
std::set<std::string> CXX::object_extensions_ = { ".o", ".a", ".lib" };

Language CXX::language() const
{
    return cxx();
}

bool CXX::accepts_file(const LanguageTypePair & key, const property::File & file) const
{
    MSS_BEGIN(bool);

    if (key.language == language())
        //Language is already known and matches with ours: we accept always
        MSS_RETURN_OK();
    
    MSS(key.language == Language::Undefined);

    //Language is not known, check if the file exists and we recognise it
    {
        const std::filesystem::path & fn = file.key();
        MSS(std::filesystem::is_regular_file(fn));
        MSS(type_from_extension(fn.extension()));
    }

    MSS_END();
}

bool CXX::resolve_file(LanguageTypePair & key, property::File & file) const
{
    MSS_BEGIN(bool);

    MSS(key.language == cxx() || key.language == Language::Undefined);
    key.language = cxx();

    // try to guess the type based on the extension
    const std::filesystem::path & fn = file.key();
    if (fn.has_extension())
        type_from_extension(key.type, fn.extension(), key.type);

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
    }



    MSS_END();
}

bool CXX::add_file(model::Recipe & recipe, const LanguageTypePair & key, const property::File & file) const
{
    MSS_BEGIN(bool);

    MSS(key.language == cxx());
    MSS(recipe.insert_or_merge(key, file));

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

bool CXX::add_additional_path_(model::Recipe & recipe, const property::File & file, Type type, Propagation propagation) const
{
    MSS_BEGIN(bool);

    const LanguageTypePair key(cxx(), type);
    auto p = recipe.insert(LanguageTypePair(cxx(), type), property::File(file.dir(), std::filesystem::path()));

    {
        property::File & include_path = *p.first;

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

        MSS_Q(src != Type::Undefined);
    }

    dst = src;

    MSS_END();
}

} }
