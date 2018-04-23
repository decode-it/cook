#ifndef HEADER_cook_rules_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_rules_Interface_hpp_ALREADY_INCLUDED

#include "cook/LanguageTypePair.hpp"
#include "cook/ingredient/File.hpp"
#include "gubg/std/filesystem.hpp"
#include <memory>

namespace cook { namespace model {

class Recipe;

}

namespace rules {

class Interface
{
public:
    using Ptr = std::shared_ptr<Interface>;

    virtual ~Interface(){}

    virtual Language language() const = 0;
    virtual bool accepts_file(const LanguageTypePair & key, const ingredient::File & file) const = 0;
    virtual Result resolve_file(LanguageTypePair & key, ingredient::File & file) const = 0;
    virtual Result add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const = 0;
    virtual void remove_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const;
};


} }

#endif
