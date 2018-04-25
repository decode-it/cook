#ifndef HEADER_cook_chai_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_File_hpp_ALREADY_INCLUDED

#include "cook/chai/Ingredient.hpp"
#include "cook/ingredient/File.hpp"

namespace cook { namespace chai {

class File : public Ingredient<ingredient::File>
{
public:
    File(const LanguageTypePair & ltp, const ingredient::File & file, const Context * context);

    bool is_file() const        { return true; }
    bool is_key_value() const   { return false; }

    std::string key() const;
    std::string dir() const;
    std::string rel() const;
};

} }

#endif
