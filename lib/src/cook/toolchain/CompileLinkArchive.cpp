#include "cook/toolchain/CompileLinkArchive.hpp"
#include "cook/chef/DependentPropagation.hpp"
#include "cook/chef/IncludePath.hpp"
#include "cook/chef/LibraryPath.hpp"

namespace cook { namespace toolchain {

CompileLinkArchive::CompileLinkArchive(const std::string & name)
    : name_(name)
{
}

std::string CompileLinkArchive::name() const
{
    return name_;
}

Result CompileLinkArchive::generate_executable_processors_(std::list<Step> & steps) const
{
    MSS_BEGIN(Result);

    // dependency propagation
    {
        Step step;
        step.name = "dependency propagation";
        step.description = "Propagation of all public dependencies from the dependees";
        step.chefs.push_back(std::make_shared<chef::DependentPropagation>());

        steps.push_back(std::move(step));
    }

    // path extraction
    {
        Step step;
        step.name = "path extraction";
        step.description = "Extraction of the correct include and library paths";
        step.chefs.push_back(std::make_shared<chef::IncludePath>());
        step.chefs.push_back(std::make_shared<chef::LibraryPath>());

        steps.push_back(std::move(step));
    }

    // compilation
    {
        Step step;
        step.name = "compilation";
        step.description = "Compilation of source units";
//        step.chefs.push_back(std::make_shared<);
    };

    MSS_END();
}

Result CompileLinkArchive::generate_shared_lib_processors_(std::list<Step> & steps) const
{

}

Result CompileLinkArchive::generate_static_lib_processors_(std::list<Step> & steps) const
{

}

Result CompileLinkArchive::generate_undefined_processors_(std::list<Step> & steps) const
{

}

Result CompileLinkArchive::generate_processors(model::Recipe * recipe, std::list<Step> & steps)
{
//    switch(recipe->type())
//    {
//        case Type::Executable:
//        case Type::Library:

//        case Type::Undefined:
//    }
}


} }
