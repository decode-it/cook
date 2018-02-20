#ifndef HEADER_cook_rules_CXX_hpp_ALREADY_INCLUDED
#define HEADER_cook_rules_CXX_hpp_ALREADY_INCLUDED

#include "cook/rules/Interface.hpp"
#include "cook/LanguageTypePair.hpp"
#include "cook/ingredient/File.hpp"
#include <set>

namespace cook { namespace rules {

class CXX : public Interface
{
public:
    Language language() const override;
    bool accepts_file(const LanguageTypePair & key, const ingredient::File & file) const override;
    bool resolve_file(LanguageTypePair &key, ingredient::File &file) const override;
    bool add_file(model::Recipe & recipe, const LanguageTypePair & key, const ingredient::File & file) const override;

    static bool type_from_extension(Type &dst, const std::string & extension, Type src);
    static bool type_from_extension(const std::string & extension) {Type type = Type::Undefined; return type_from_extension(type, extension, type);}

private:
    bool add_additional_path_(model::Recipe & recipe, const ingredient::File & file, Type type, Propagation propagation) const;

    static std::set<std::string> source_extensions_;
    static std::set<std::string> header_extensions_;
    static std::set<std::string> object_extensions_;
    static std::set<std::string> lib_extensions_;
};

} }

#endif
