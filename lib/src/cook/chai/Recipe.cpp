#include "cook/chai/Recipe.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai {

Recipe::Recipe(model::Recipe * recipe, Logger *logger)
    : recipe_(recipe),
      logger_(logger),
      data_(from_any(recipe->user_data()))
{
}

void Recipe::set_type(Type type)
{
    recipe_->set_type(type);
}

void Recipe::add(const std::string & dir, const std::string & pattern)
{
    model::GlobInfo info;
    info.dir = dir;
    info.pattern = pattern;

    recipe_->add_globber(info);
}

void Recipe::depends_on(const std::string & dependency)
{
    std::pair<model::Uri, bool> p = model::Uri::recipe_uri(dependency);
    if (!p.second)
        logger_->LOG(Error, "bad uri");

    recipe_->add_dependency(p.first);
}


void Recipe::library(const std::string & library)
{
    recipe_->pre().files().insert(LanguageTypePair(Language::CXX, Type::Library), ingredient::File("", library));
}

void Recipe::library_path(const std::string & path)
{
    recipe_->pre().files().insert(LanguageTypePair(Language::CXX, Type::LibraryPath), ingredient::File(path, ""));
}

void Recipe::include_path(const std::string & path)
{
    recipe_->pre().files().insert(LanguageTypePair(Language::CXX, Type::IncludePath), ingredient::File(path, ""));
}

gubg::chai::ModulePtr recipe_module()
{
    gubg::chai::ModulePtr ptr = std::make_unique<chaiscript::Module>();

    ptr->add(chaiscript::user_type<Recipe>(), "Recipe");
    ptr->add(chaiscript::fun(&Recipe::add), "add");
    ptr->add(chaiscript::fun(&Recipe::depends_on), "depends_on");
    ptr->add(chaiscript::fun(&Recipe::set_type), "set_type");
    ptr->add(chaiscript::fun(&Recipe::library), "library");
    ptr->add(chaiscript::fun(&Recipe::library_path), "library_path");
    ptr->add(chaiscript::fun(&Recipe::include_path), "include_path");
    ptr->add(chaiscript::fun(&Recipe::data), "data");

    return ptr;
}


} }
