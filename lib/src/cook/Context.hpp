#ifndef HEADER_cook_Context_hpp_ALREADY_INCLUDED
#define HEADER_cook_Context_hpp_ALREADY_INCLUDED

#include "cook/Logger.hpp"
#include "cook/model/Book.hpp"
#include "cook/model/Library.hpp"
#include "cook/model/Dirs.hpp"
#include "cook/process/Menu.hpp"
#include "cook/visualizer/Interface.hpp"

namespace cook {

class Context
{
public:
    using VisualizerPtr = std::shared_ptr<visualizer::Interface>;
    using Variable = std::pair<std::string, std::string>;

    virtual ~Context() {}

    bool initialize();

    virtual Logger & logger() = 0;

    Result initialize_menu(const std::list<model::Recipe*> & root_recipes);


    // getters and setter
    model::Book * root_book() const;
    model::Dirs & dirs()                { return dirs_; }
    const model::Dirs & dirs() const    { return dirs_; }
    const process::Menu & menu() const  { return menu_; }
    const model::Library & lib() const  { return lib_; }

    Result register_visualizer(VisualizerPtr visualizer);
    VisualizerPtr get_visualizer(const std::string & name) const;

    Result find_recipe(model::Recipe *&recipe, const std::string & name) const;

    virtual Result set_variable(const std::string & name, const std::string & value) = 0;

private:
    std::map<std::string, VisualizerPtr> visualizers_;
    model::Library lib_;
    model::Dirs dirs_;
    process::Menu menu_;
};

}

#endif
