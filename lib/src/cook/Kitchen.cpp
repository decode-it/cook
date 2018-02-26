#include "cook/Kitchen.hpp"
#include "cook/visualizer/Graphviz.hpp"
#include "gubg/mss.hpp"
#include <cassert>

namespace  { 
    /* const char *logns = ""; */
    const char *logns = nullptr;
} 

namespace cook {

Kitchen::Kitchen()
{
}

bool Kitchen::initialize()
{
    MSS_BEGIN(bool, logns);
    context_.environment = create_environment();
    MSS(!!context_.environment);

    // add the visualizer
    MSS(register_visualizer(std::make_shared<visualizer::Graphviz>()));

    MSS_END();

}

Result Kitchen::register_toolchain(ToolchainPtr toolchain)
{
    MSS_BEGIN(Result, logns);

    MSS(!!toolchain);
    const std::string & name = toolchain->name();

    MSG_MSS(!name.empty(), Error, "Cannot add a toolchain with an empty name");
    MSG_MSS(toolchains_.find(name) == toolchains_.end(), Error, "A toolchain with name '" << name << "' already exists");

    toolchains_.emplace(name, toolchain);

    MSS_END();
}

Result Kitchen::register_visualizer(VisualizerPtr visualizer)
{
    MSS_BEGIN(Result, logns);

    MSS(!!visualizer);
    const std::string & name = visualizer->name();

    MSG_MSS(!name.empty(), Error, "Cannot add a visualizer with an empty name");
    MSG_MSS(visualizers_.find(name) == visualizers_.end(), Error, "A visualizer with name '" << name << "' already exists");

    visualizers_.emplace(name, visualizer);

    MSS_END();
}

Kitchen::ToolchainPtr Kitchen::get_toolchain(const std::string & name) const
{
    auto it = toolchains_.find(name);
    return it == toolchains_.end() ? ToolchainPtr() : it->second;
}

Kitchen::VisualizerPtr Kitchen::get_visualizer(const std::string & name) const
{
    auto it = visualizers_.find(name);
    return it == visualizers_.end() ? VisualizerPtr() : it->second;
}

model::Context & Kitchen::context()
{
    return context_;
}

const model::Context & Kitchen::context() const
{
    return context_;
}

model::Book * Kitchen::root_book() const
{
    return context_.root;
}

Result Kitchen::register_variable(const std::string &name, const std::string &value)
{
    MSS_BEGIN(Result, logns);

    MSS(!!context_.environment);
    MSS(context_.environment->set_variable(name, value));

    MSS_END();
}


Result Kitchen::find_recipe(model::Recipe *& recipe, const std::string & name) const
{
    MSS_BEGIN(Result, logns);

    // create the uri
    std::pair<model::Uri, bool> p = model::Uri::recipe_uri(name);

    MSG_MSS(p.second, Error, "Invalid recipe uri '" << name << "'");

    // forgive the fact that recipe is not absolute
    p.first.set_absolute(true);

    // find the recipe in the root book
    MSG_MSS(model::find_recipe(recipe, root_book(), p.first), InternalError, "Error while finding uri '" << p.first << "'");

    // make sure the recipe is found
    MSG_MSS(!!recipe, Error, "No recipe with name '" << p.first << "' exists ");

    MSS_END();

}

Result Kitchen::prepare(const model::Menu & menu)
{
    MSS_BEGIN(Result, logns);
    MSS_END();
}

}


