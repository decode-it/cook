#ifndef HEADER_cook_process_souschef_Resolver_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_souschef_Resolver_hpp_ALREADY_INCLUDED

#include "cook/process/souschef/Interface.hpp"
#include "cook/rules/RuleSet.hpp"
#include "cook/model/GlobInfo.hpp"
#include "cook/ingredient/File.hpp"

namespace cook { namespace process { namespace souschef { 

    class Resolver: public Interface
    {
    public:
        Resolver(const rules::RuleSet::Ptr &rule_set)
        : rule_set_(rule_set)
        {
        }
        virtual ~Resolver() {}

        Result process(const Context &, model::Recipe &) const override;
        std::string description() const override;

        bool process_one(model::Recipe & recipe, const model::GlobInfo & globber) const;

    private:
        bool process_(model::Recipe & recipe, LanguageTypePair & key, ingredient::File & file) const;

        rules::RuleSet::Ptr rule_set_;
    };

} } }

#endif
