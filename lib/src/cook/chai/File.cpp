#include "cook/chai/File.hpp"

namespace cook { namespace chai {

File::File(const LanguageTypePair & ltp, const ingredient::File & file, const Context *context)
    : Ingredient<ingredient::File>(ltp, file, context)
{
}

const std::string & File::key() const
{
    return element().key();
}

std::string File::dir() const
{
    return element().dir().string();
}
std::string File::rel() const
{
    return element().rel().string();
}

} }
