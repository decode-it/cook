#ifndef HEADER_cook_chai_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Recipe_hpp_ALREADY_INCLUDED

#include "cook/chai/UserData.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/Logger.hpp"
#include "gubg/chai/ModuleFwd.hpp"

namespace cook { namespace chai {

class Recipe
{
public:
    Recipe(model::Recipe * recipe, Logger * runner);

    void add(const std::string & dir, const std::string & pattern);
    void depends_on(const std::string & dependency);
    void set_type(cook::Type type);

    void library(const std::string & library);
    void library_path(const std::string & path);
    void include_path(const std::string & path);

    UserData data() const { return data_; }

private:
    model::Recipe * recipe_;
    Logger * logger_;
    UserData data_;
};

gubg::chai::ModulePtr recipe_module();

} }

#endif
