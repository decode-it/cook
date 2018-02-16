#ifndef HEADER_cook_rules_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_rules_Interface_hpp_ALREADY_INCLUDED

#include "cook/LanguageTypePair.hpp"
#include "cook/property/File.hpp"
#include "gubg/std/filesystem.hpp"

namespace cook { namespace model {

class Recipe;

}

namespace rules {

class Interface
{
public:
    virtual ~Interface(){}

    virtual Language language() const = 0;
    virtual bool accepts_file(const LanguageTypePair & key, const property::File & file) const = 0;
    virtual bool resolve_file(LanguageTypePair & key, property::File & file) const = 0;
    virtual bool add_file(model::Recipe & recipe, const LanguageTypePair & key, const property::File & file) const = 0;
};


} }

#endif
