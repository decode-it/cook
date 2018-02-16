#ifndef HEADER_cook_rules_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_rules_Interface_hpp_ALREADY_INCLUDED

#include "cook/LanguageTypePair.hpp"
#include "cook/property/File.hpp"
#include "gubg/std/filesystem.hpp"

namespace cook { namespace rules {

class Interface
{
public:
    virtual ~Interface(){}

    virtual Language language() const = 0;
    virtual bool adapt(LanguageTypePair & key, property::File & file) const = 0;
};


} }

#endif
