#include "cook/chai/Recipe.hpp"
#include "cook/chai/Context.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai {

Recipe::Recipe(model::Recipe * recipe, Context *context, Logger *logger)
    : recipe_(recipe),
      context_(context),
      logger_(logger),
      data_(from_any(recipe->user_data()))
{
}

void Recipe::set_type(model::Recipe::Type type)
{
    recipe_->set_type(type);
}

void Recipe::set_working_directory(const std::string & dir)
{
    recipe_->set_working_directory(dir);
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
    auto s = log::scope("depends on", [&](auto & n) { n.attr("uri", recipe_->uri().string()).attr("dependency", dependency); });

    std::pair<model::Uri, bool> p = model::Uri::recipe_uri(dependency);
    if (!p.second)
        logger_->log(Message::Error, "bad uri");

    recipe_->add_dependency(p.first);
}


void Recipe::library(const std::string & library)
{
    auto lib = ingredient::KeyValue(library);
    lib.set_propagation(Propagation::Public);
    lib.set_overwrite(Overwrite::IfSame);

    recipe_->key_values().insert(LanguageTypePair(Language::Binary, Type::Library), lib);
}

void Recipe::library_path(const std::string & path)
{
    auto lib_path = ingredient::File(path, {});
    lib_path.set_propagation(Propagation::Public);
    lib_path.set_overwrite(Overwrite::IfSame);

    recipe_->files().insert(LanguageTypePair(Language::Binary, Type::LibraryPath), lib_path);
}

Language Recipe::guess_language_() const
{
    return Language::Undefined;
}

void Recipe::include_path(const std::string & path)
{
    auto inc_path = ingredient::File(path, {});

    inc_path.set_propagation(Propagation::Public);
    inc_path.set_overwrite(Overwrite::IfSame);

    recipe_->files().insert(LanguageTypePair(guess_language_(), Type::IncludePath), inc_path);
}

void Recipe::define_1(const std::string & name)
{
    auto kv = ingredient::KeyValue(name);
    kv.set_propagation(Propagation::Public);
    kv.set_overwrite(Overwrite::IfSame);

    recipe_->key_values().insert(LanguageTypePair(guess_language_(), Type::Define), kv);
}

void Recipe::define_2(const std::string & name, const std::string & value)
{
    auto kv = ingredient::KeyValue(name, value);
    kv.set_propagation(Propagation::Public);
    kv.set_overwrite(Overwrite::IfSame);

    recipe_->key_values().insert(LanguageTypePair(guess_language_(), Type::Define), kv);
}

gubg::chai::ModulePtr recipe_module()
{
    gubg::chai::ModulePtr ptr = std::make_unique<chaiscript::Module>();

    return ptr;
}


} }
