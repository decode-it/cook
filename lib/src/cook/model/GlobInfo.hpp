#ifndef HEADER_cook_model_GlobInfo_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_GlobInfo_hpp_ALREADY_INCLUDED

#include "cook/property/File.hpp"
#include "cook/LanguageTypePair.hpp"
#include "cook/Propagation.hpp"
#include "cook/Overwrite.hpp"
#include "cook/Language.hpp"
#include "cook/Type.hpp"
#include "gubg/file/System.hpp"
#include <string>
#include <optional>
#include <functional>
#include <algorithm>


namespace cook { namespace model {

struct GlobInfo
{
    std::string dir;
    std::string pattern;

    Language language = Language::Undefined;
    Type type = Type::Undefined;

    std::optional<Propagation> propagation;
    std::optional<Overwrite> overwrite;
    std::function<bool (LanguageTypePair &, property::File &)> filter_and_adaptor;
};

} }

#endif
