#ifndef HEADER_cook_Context_hpp_ALREADY_INCLUDED
#define HEADER_cook_Context_hpp_ALREADY_INCLUDED

#include "cook/Logger.hpp"
#include "cook/OS.hpp"
#include "cook/model/Book.hpp"
#include "cook/model/Library.hpp"
#include "cook/model/Dirs.hpp"
#include "cook/process/Menu.hpp"
#include "boost/graph/adjacency_list.hpp"
#include <optional>

namespace cook {

    namespace generator {
        class Interface;
    }
    namespace process {
        namespace toolchain { 
            class Manager;
        } 
    } 

    class Context
    {
    public:
        using GeneratorPtr = std::shared_ptr<generator::Interface>;
        using Variable = std::pair<std::string, std::string>;
        using ToolchainManagerPtr = std::shared_ptr<process::toolchain::Manager>;

        virtual ~Context() {}

        Result initialize();

        virtual const Logger & logger() const = 0;

        Result initialize_menu(const std::list<model::Recipe*> & root_recipes);

        // getters and setter
        model::Book * root_book() const;
        model::Dirs & dirs()                        { return dirs_; }
        const model::Dirs & dirs() const            { return dirs_; }
        const process::Menu & menu() const          { return menu_; }
        process::Menu & menu()                      { return menu_; }
        const model::Library & lib() const          { return lib_; }
        const std::string & project_name() const    { return project_name_; }
        void set_project_name(const std::string & name) { project_name_ = name; }
        OS os() const;

        Result set_toolchain(const std::string &toolchain);
        const process::toolchain::Manager &toolchain() const;

        Result register_generator(GeneratorPtr generator);
        GeneratorPtr get_generator(const std::string & name) const;

        Result find_recipe(model::Recipe *&recipe, const std::string & name) const;

        virtual Result set_variable(const std::string & name, const std::string & value) = 0;

    private:
        std::map<std::string, GeneratorPtr> generators_;
        model::Library lib_;
        model::Dirs dirs_;
        process::Menu menu_;
        std::string project_name_;

        mutable ToolchainManagerPtr toolchain_ptr_;
        process::toolchain::Manager &toolchain_() const;
    };

}

#endif
