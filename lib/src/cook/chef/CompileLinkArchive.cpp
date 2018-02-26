#include "cook/chef/CompileLinkArchive.hpp"
#include "cook/chef/assistant/DependencyPropagator.hpp"
#include "cook/chef/assistant/IncludePathSetter.hpp"
#include "cook/chef/assistant/LibraryPathSetter.hpp"
#include "cook/chef/assistant/Compiler.hpp"
#include "cook/chef/assistant/Archiver.hpp"
#include "cook/chef/assistant/Linker.hpp"
#include "cook/chef/assistant/LinkLibrarySorter.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace chef {

namespace  {

const static std::initializer_list<Language> default_supported_languages = { Language::C, Language::CXX };

}

LinkArchiveChef::LinkArchiveChef(const std::string & name)
    : name_(name)
{
    linker_     = std::make_shared<assistant::Linker>();
    archiver_   = std::make_shared<assistant::Archiver>();

    for (cook::Language Language : default_supported_languages)
        compilers_[Language] = std::make_shared<assistant::Compiler>(Language);
}

Result LinkArchiveChef::initialize()
{
    // the linker
    {
        InstructionSet set;

        set.name = "Link executable or dynamic library";
        set.can_cook = [](const model::Recipe * recipe, std::string & reason) {
            if (recipe->type() == Type::Library || recipe->type() == Type::Executable)
                return true;

            reason = gubg::stream([](auto & os) { os << "Only recipes of type " << Type::Library << " or " << Type::Executable << " will be linked"; });
            return false;
        };

        set.assistants = generate_compile_only_steps_();
        set.assistants.push_back(linker_);

        this->add_instruction_set(100, std::move(set));
    }

    // the archiver
    {
        InstructionSet set;

        set.name = "Archive static library";
        set.can_cook = [](const model::Recipe * recipe, std::string & reason) {
            if (recipe->type() == Type::Undefined)
                return true;

            reason = gubg::stream([](auto & os) { os << "Only recipes of type " << Type::Undefined << " will be archived"; });
            return false;
        };

        set.assistants = generate_compile_only_steps_();
        set.assistants.push_back(archiver_);

        this->add_instruction_set(100, std::move(set));
    }
}

void LinkArchiveChef::set_compiler(Language language, AssistantPtr compiler)
{
    compilers_[language] = compiler;
}

void LinkArchiveChef::clear_compiler(Language language)
{
    compilers_.erase(language);
}

void LinkArchiveChef::set_linker(AssistantPtr linker)
{
    linker_ = linker;
}

void LinkArchiveChef::set_archiver(AssistantPtr archiver)
{
    archiver_ = archiver;
}

std::list<AssistantPtr> LinkArchiveChef::generate_compile_only_steps_() const
{
    std::list<AssistantPtr> result;

    result.push_back(std::make_shared<assistant::DependentPropagator>());

    for (const auto & p : compilers_)
        result.push_back(std::make_shared<assistant::IncludePathSetter>(p.first));

    result.push_back(std::make_shared<assistant::LibraryPathSetter>());

    for (const auto & p : compilers_)
        result.push_back(p.second);

    return result;
}

} }
