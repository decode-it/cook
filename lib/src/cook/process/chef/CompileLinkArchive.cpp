#include "cook/process/chef/CompileLinkArchive.hpp"
#include "cook/process/souschef/Resolver.hpp"
#include "cook/process/souschef/DependencyPropagator.hpp"
#include "cook/process/souschef/IncludePathSetter.hpp"
#include "cook/process/souschef/LibraryPathSetter.hpp"
#include "cook/process/souschef/Compiler.hpp"
#include "cook/process/souschef/Archiver.hpp"
#include "cook/process/souschef/Linker.hpp"
#include "cook/process/souschef/RecipeNamer.hpp"
#include "cook/process/souschef/LinkLibrarySorter.hpp"
#include "cook/rules/CXX.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace process { namespace chef {

namespace  {

const static unsigned int default_priority = 100;

const static std::initializer_list<Language> default_supported_languages = { Language::C, Language::CXX };

}

LinkArchiveChef::LinkArchiveChef(const std::string & name)
    : name_(name)
{
    linker_     = std::make_shared<souschef::Linker>();
    archiver_   = std::make_shared<souschef::Archiver>();

    for (cook::Language Language : default_supported_languages)
        compilers_[Language] = std::make_shared<souschef::Compiler>(Language);
}

Result LinkArchiveChef::initialize()
{
    MSS_BEGIN(Result);

    // the linker
    {
        Brigade set;

        set.name = "Link executable or dynamic library";
        set.can_cook = [](const model::Recipe * recipe, std::string & reason) {
            if (recipe->type() == Type::Library || recipe->type() == Type::Executable)
                return true;

            reason = gubg::stream([](auto & os) { os << "Only recipes of type " << Type::Library << " or " << Type::Executable << " will be linked"; });
            return false;
        };

        set.souschefs = generate_compile_only_steps_();
        set.souschefs.push_back(std::make_shared<souschef::LinkLibrarySorter>());
        set.souschefs.push_back(linker_);

        this->add_brigade(default_priority, std::move(set));
    }

    // the archiver
    {
        Brigade set;

        set.name = "Archive static library";
        set.can_cook = [](const model::Recipe * recipe, std::string & reason) {
            if (recipe->type() == Type::Undefined)
                return true;

            reason = gubg::stream([](auto & os) { os << "Only recipes of type " << Type::Undefined << " will be archived"; });
            return false;
        };

        set.souschefs = generate_compile_only_steps_();
        set.souschefs.push_back(archiver_);

        this->add_brigade(default_priority, std::move(set));
    }

    MSS_END();
}

void LinkArchiveChef::set_compiler(Language language, SouschefPtr compiler)
{
    compilers_[language] = compiler;
}

void LinkArchiveChef::clear_compiler(Language language)
{
    compilers_.erase(language);
}

void LinkArchiveChef::set_linker(SouschefPtr linker)
{
    linker_ = linker;
}

void LinkArchiveChef::set_archiver(SouschefPtr archiver)
{
    archiver_ = archiver;
}

std::list<SouschefPtr> LinkArchiveChef::generate_compile_only_steps_() const
{
    std::list<SouschefPtr> result;

    auto rule_set = rules::RuleSet::create();
    rule_set->add<rules::CXX>();
    result.push_back(std::make_shared<souschef::Resolver>(rule_set));

    result.push_back(std::make_shared<souschef::DependentPropagator>());

    for (const auto & p : compilers_)
        result.push_back(std::make_shared<souschef::IncludePathSetter>(p.first));

    result.push_back(std::make_shared<souschef::LibraryPathSetter>());

    for (const auto & p : compilers_)
        result.push_back(p.second);

    result.push_back(std::make_shared<souschef::RecipeNamer>());

    return result;
}

} } }
