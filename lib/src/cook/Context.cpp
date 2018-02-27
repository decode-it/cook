#include "cook/Context.hpp"
#include "cook/visualizer/Graphviz.hpp"
#include "gubg/mss.hpp"
#include <cassert>

namespace  { 
    /* const char *logns = ""; */
    const char *logns = nullptr;
} 

namespace cook {

Context::Context()
    : root_(std::make_shared<model::Book>())
{
}

bool Context::initialize()
{
    MSS_BEGIN(bool);
    environment_ = create_environment();
    MSS(!!environment_);

    // add the visualizer
    MSS(register_visualizer(std::make_shared<visualizer::Graphviz>()));

    MSS_END();

}

Result Context::register_visualizer(VisualizerPtr visualizer)
{
    MSS_BEGIN(Result, logns);

    MSS(!!visualizer);
    const std::string & name = visualizer->name();

    MSG_MSS(!name.empty(), Error, "Cannot add a visualizer with an empty name");
    MSG_MSS(visualizers_.find(name) == visualizers_.end(), Error, "A visualizer with name '" << name << "' already exists");

    visualizers_.emplace(name, visualizer);

    MSS_END();
}

Context::VisualizerPtr Context::get_visualizer(const std::string & name) const
{
    auto it = visualizers_.find(name);
    return it == visualizers_.end() ? VisualizerPtr() : it->second;
}


model::Book * Context::root_book() const
{
    return root_.get();
}
model::Environment * Context::environment() const
{
    return environment_.get();
}

Result Context::register_variable(const std::string &name, const std::string &value)
{
    MSS_BEGIN(Result, logns);

    MSS(!!environment_);
    MSS(environment_->set_variable(name, value));

    MSS_END();
}


Result Context::find_recipe(model::Recipe *& recipe, const std::string & name) const
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

}


