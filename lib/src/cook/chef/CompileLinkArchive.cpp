#include "cook/chef/CompileLinkArchive.hpp"
#include "cook/chef/assistant/DependencyPropagator.hpp"
#include "cook/chef/assistant/IncludePathSetter.hpp"
#include "cook/chef/assistant/LibraryPathSetter.hpp"
#include "cook/chef/assistant/Compiler.hpp"
#include "cook/chef/assistant/Archiver.hpp"
#include "cook/chef/assistant/Linker.hpp"
#include "cook/chef/assistant/LinkLibrarySorter.hpp"

namespace cook { namespace chef {

CompileLinkArchive::CompileLinkArchive(const std::string & name)
    : name_(name),
      linker_(std::make_shared<assistant::Linker>()),
      archiver_(std::make_shared<assistant::Archiver>())
{
    for (Language language : {Language::C, Language::CXX})
        compilers_[language] = std::make_shared<assistant::Compiler>(language);
}

std::string CompileLinkArchive::name() const
{
    return name_;
}

Result CompileLinkArchive::initialize()
{
    MSS_BEGIN(Result);

    compile_only_steps_ = generate_compile_only_steps_();
    archive_steps_ = generate_archive_steps_();
    link_steps_ = generate_link_steps_();

    MSS_END();
}

Result CompileLinkArchive::generate_processors(model::Recipe * recipe, std::list<Step> & steps)
{
    MSS_BEGIN(Result);

    switch(recipe->type())
    {
        case Type::Executable:
        case Type::Library:
            steps.assign(link_steps_.begin(), link_steps_.end());
            break;

        case Type::Undefined:
            steps.assign(archive_steps_.begin(), archive_steps_.end());
            break;

        default:
            MSG_MSS(false, Error, recipe->uri() << ": Toolchain does not support type " << recipe->type() << "");
    }

    MSS_END();
}

void CompileLinkArchive::disable_build()
{

}


CompileLinkArchive::StepList CompileLinkArchive::generate_compile_only_steps_() const
{
    StepList steps;

    // dependency propagation
    {
        Step step;
        step.name = "dependency propagation";
        step.description = "Propagation of all public dependencies from the dependees";
        step.chefs.push_back(std::make_shared<assistant::DependentPropagator>());

        steps.push_back(std::move(step));
    }

    // path extraction
    {
        Step step;
        step.name = "path extraction";
        step.description = "Extraction of the correct include and library paths";

        // include path extractor for every language
        for (const auto & p: compilers_)
            step.chefs.push_back(std::make_shared<assistant::IncludePathSetter>(p.first));

        step.chefs.push_back(std::make_shared<assistant::LibraryPathSetter>());

        steps.push_back(std::move(step));
    }

    // compilation
    {
        Step step;
        step.name = "compilation";
        step.description = "Compilation of source units";

        for (const auto & p: compilers_)
            step.chefs.push_back(p.second);

        steps.push_back(std::move(step));
    };
}
CompileLinkArchive::StepList CompileLinkArchive::generate_archive_steps_() const
{
    StepList steps = generate_compile_only_steps_();

    // archiver
    {
        Step step;
        step.name =  "archiving";
        step.description = "Creating an archive from object code";
        step.chefs.push_back(archiver_);

        steps.push_back(std::move(step));
    }

    return steps;
}

CompileLinkArchive::StepList CompileLinkArchive::generate_link_steps_() const
{
    StepList steps = generate_compile_only_steps_();

    // Ordering the link libraries
    {
        Step step;
        step.name = "sorting libraries";
        step.description = "Sorting the link libraries in topological order";
        step.chefs.push_back(std::make_shared<assistant::LinkLibrarySorter>());

        steps.push_back(std::move(step));
    }

    // linker
    {
        Step step;
        step.name =  "Linking";
        step.description = "Linking all object code";
        step.chefs.push_back(linker_);

        steps.push_back(std::move(step));
    }

    return steps;
}



} }
