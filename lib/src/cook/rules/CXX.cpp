#include "cook/rules/CXX.hpp"

namespace cook { namespace rules {

std::set<std::string> CXX::extensions_ = { "cpp", "cxx", "hpp", "hxx" };

Language CXX::language() const
{
    return Language::CXX;
}

bool CXX::accepts(const LanguageTypePair & key, const property::File & file) const
{
    const std::filesystem::path & fn = file.key();

    if (!std::filesystem::is_regular_file(fn) || !fn.has_extension())
        return false;

    if (key.language != language() && key.language != Language::Undefined)
        return false;

    return extensions_.find(fn.extension()) != extensions_.end();
}

} }
