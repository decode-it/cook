#ifndef HEADER_cook_model_Globber_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Globber_hpp_ALREADY_INCLUDED

#include "cook/model/property/File.hpp"
#include "cook/Propagation.hpp"
#include "cook/Overwrite.hpp"
#include "cook/Language.hpp"
#include "cook/Type.hpp"
#include <string>
#include <optional>
#include <functional>

namespace cook { namespace model {

struct Globber
{
    std::string dir;
    std::string rel;

    std::optional<Propagation> propagation;
    std::optional<Overwrite> overwrite;
    std::optional<Language> language;
    std::optional<Type> type;
    std::function<bool (Language &, Type &, property::File &)> filter_and_adaptor;
};

} }

#endif
