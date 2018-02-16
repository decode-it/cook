#include "cook/rules/CXX.hpp"
#include "cook/model/Recipe.hpp"

namespace cook { namespace rules {

namespace {

static Language cxx() { return Language::CXX; }

}

std::set<std::string> CXX::source_extensions_ = { "cpp", "cxx", "CPP", "CXX" };
std::set<std::string> CXX::header_extensions_ = { "hpp", "hxx", "HPP", "HXX" };
std::set<std::string> CXX::object_extensions_ = { "o", "a", "lib" };

Language CXX::language() const
{
    return cxx();
}


bool CXX::accepts_file(const LanguageTypePair & key, const property::File & file) const
{
    // language is either correct or undefined
    if (false) {}
    // accept everything for which the language is already defined
    else if (key.language == cxx())
        return true;
    else if (key.language != Language::Undefined)
        return false;

    // existing file with the right type of extension
    const std::filesystem::path & fn = file.key();
    return std::filesystem::is_regular_file(fn)
            && fn.has_extension()
            && type_from_extension(fn.extension());
}

bool CXX::resolve_file(LanguageTypePair & key, property::File & file) const
{
    MSS_BEGIN(bool);

    MSS(key.language == cxx() || key.language == Language::Undefined);
    key.language = cxx();

    // try to guess the type based on the extension
    const std::filesystem::path & fn = file.key();
    if (fn.has_extension())
    {
        auto p = type_from_extension(fn.extension(), key.type);
        if (p)
            key.type = *p;
    }

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



std::optional<Type> CXX::type_from_extension(const std::string & extension, Type type)
{
    if (false) {}
    else if (type != Type::Undefined)
        return type;
    else if (source_extensions_.find(extension) != source_extensions_.end())
        return Type::Source;
    else if (header_extensions_.find(extension) != header_extensions_.end())
        return Type::Header;
    else if (object_extensions_.find(extension) != object_extensions_.end())
        return Type::Object;

    return {};
}

} }
