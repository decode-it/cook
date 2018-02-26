#ifndef HEADER_cook_Kitchen_hpp_ALREADY_INCLUDED
#define HEADER_cook_Kitchen_hpp_ALREADY_INCLUDED

#include "cook/Logger.hpp"
#include "cook/model/Book.hpp"
#include "cook/model/Menu.hpp"
#include "cook/chef/Interface.hpp"
#include "cook/visualizer/Interface.hpp"
#include "cook/model/Context.hpp"

namespace cook {

class Kitchen
{
public:
    using ToolchainPtr = std::shared_ptr<chef::Interface>;
    using VisualizerPtr = std::shared_ptr<visualizer::Interface>;
    using Variable = std::pair<std::string, std::string>;

    Kitchen();
    virtual ~Kitchen() {}

    bool initialize();

    virtual Logger & logger() = 0;

    // getters and setter
    model::Book * root_book() const;
    model::Environment * environment() const;

    Result register_toolchain(ToolchainPtr chef);
    Result register_visualizer(VisualizerPtr visualizer);
    ToolchainPtr get_toolchain(const std::string & name) const;
    VisualizerPtr get_visualizer(const std::string & name) const;

    Result register_variable(const std::string & name, const std::string & value);
    Result find_recipe(model::Recipe *&recipe, const std::string & name) const;

private:
    virtual std::shared_ptr<model::Environment> create_environment() const = 0;
    std::map<std::string, ToolchainPtr> toolchains_;
    std::map<std::string, VisualizerPtr> visualizers_;

    std::shared_ptr<model::Book> root_;
    std::shared_ptr<model::Environment> environment_;

};

}

#endif
