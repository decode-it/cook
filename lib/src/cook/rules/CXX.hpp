#ifndef HEADER_cook_rules_CXX_hpp_ALREADY_INCLUDED
#define HEADER_cook_rules_CXX_hpp_ALREADY_INCLUDED

#include "cook/rules/Interface.hpp"
#include "cook/LanguageTypePair.hpp"
#include "cook/property/File.hpp"
#include <set>
#include <optional>

namespace cook { namespace rules {

class CXX : public Interface
{
public:
    Language language() const override;
    bool accepts_file(const LanguageTypePair & key, const property::File & file) const override;
    bool resolve_file(LanguageTypePair &key, property::File &file) const override;
    bool add_file(model::Recipe & recipe, const LanguageTypePair & key, const property::File & file) const override;

private:
    bool add_additional_path_(model::Recipe & recipe, const property::File & file, Type type, Propagation propagation) const;

    static std::optional<Type> type_from_extension(const std::string & extension, Type type = Type::Undefined);

    static std::set<std::string> source_extensions_;
    static std::set<std::string> header_extensions_;
    static std::set<std::string> object_extensions_;
};

} }

#endif
