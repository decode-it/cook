#include "cook/process/chef/CompileArchiveLink.hpp"
#include "cook/process/souschef/Resolver.hpp"
#include "cook/process/souschef/DependencyPropagator.hpp"
#include "cook/process/souschef/IncludePathSetter.hpp"
#include "cook/process/souschef/Compiler.hpp"
#include "cook/process/souschef/Archiver.hpp"
#include "cook/process/souschef/Linker.hpp"
#include "cook/process/souschef/RecipeNamer.hpp"
#include "cook/process/souschef/LinkLibrarySorter.hpp"
#include "cook/process/souschef/ScriptRunner.hpp"
#include "cook/rules/C_family.hpp"
#include "cook/rules/ASM.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace process { namespace chef {

namespace  {

const static unsigned int default_priority = 100;

const static std::initializer_list<Language> default_supported_languages = { Language::C, Language::CXX, Language::ASM };

}

CompileArchiveLink::CompileArchiveLink(const std::string & name)
    : name_(name)
{
    linker_     = std::make_shared<souschef::Linker>();
    archiver_   = std::make_shared<souschef::Archiver>();

    for (cook::Language Language : default_supported_languages)
        compilers_[Language] = std::make_shared<souschef::Compiler>(Language);
}

Result CompileArchiveLink::initialize()
{
    MSS_BEGIN(Result);

    // the library linker
    {
        Brigade set;

        set.name = "Link dynamic library";
        set.can_cook = [](const model::Recipe * recipe, std::string & reason) {
            if (recipe->build_target().type == TargetType::SharedLibrary || recipe->build_target().type == TargetType::Executable)
                return true;

            reason = gubg::stream([](auto & os) { os << "Only recipes of type " << TargetType::SharedLibrary << " or " << TargetType::Executable << " will be linked"; });
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
            if (recipe->build_target().type == TargetType::Archive)
                return true;

            reason = gubg::stream([](auto & os) { os << "Only recipes of type " << TargetType::Archive << " will be archived"; });
            return false;
        };

        set.souschefs = generate_compile_only_steps_();
        set.souschefs.push_back(archiver_);

        this->add_brigade(default_priority, std::move(set));
    }

    // the compiler
    {
        Brigade set;

        set.name = "Compile object files";
        set.can_cook = [](const model::Recipe * recipe, std::string & reason) {
            if (recipe->build_target().type == TargetType::Undefined)
                return true;

            return false;
        };

        set.souschefs = generate_compile_only_steps_();

        this->add_brigade(default_priority-1, std::move(set));
    }

    // the script executor
    {
        Brigade set;
        set.name = "Script executor";
        set.can_cook = [](const model::Recipe * recipe, std::string & reason) {

            if (recipe->build_target().type != TargetType::Script)
            {
                reason = gubg::stream([](auto & os) {os << "Only recipes of type " << TargetType::Script << " can be executed"; });
                return false;
            }

            //TODO: In the future, this should be allowed.
            if (!recipe->dependency_pairs().empty())
            {
                reason = gubg::stream([](auto & os) {os << "A script recipe cannot have dependencies"; });
                return false;
            }

            return true;
        };

        set.souschefs.push_back(std::make_shared<souschef::ScriptRunner>());
        this->add_brigade(default_priority-2, std::move(set));
    }

    MSS_END();
}

void CompileArchiveLink::set_compiler(Language language, SouschefPtr compiler)
{
    compilers_[language] = compiler;
}

void CompileArchiveLink::clear_compiler(Language language)
{
    compilers_.erase(language);
}

void CompileArchiveLink::set_linker(SouschefPtr linker)
{
    linker_ = linker;
}

void CompileArchiveLink::set_archiver(SouschefPtr archiver)
{
    archiver_ = archiver;
}

std::list<SouschefPtr> CompileArchiveLink::generate_compile_only_steps_() const
{
    std::list<SouschefPtr> result;

    auto rule_set = rules::RuleSet::create();
    rule_set->add<rules::CXX>();
    rule_set->add<rules::Cc>();
    rule_set->add<rules::ASM>();
    result.push_back(std::make_shared<souschef::Resolver>(rule_set));

    result.push_back(std::make_shared<souschef::DependentPropagator>());

    for (const auto & p : compilers_)
        result.push_back(std::make_shared<souschef::IncludePathSetter>(p.first));

    for (const auto & p : compilers_)
        result.push_back(p.second);

    result.push_back(std::make_shared<souschef::RecipeNamer>());

    return result;
}

} } }
