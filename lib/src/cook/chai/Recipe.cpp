#include "cook/chai/Recipe.hpp"
#include "cook/chai/Context.hpp"
#include "cook/chai/mss.hpp"

namespace cook { namespace chai {

Recipe::Recipe(model::Recipe * recipe, Context *context)
    : recipe_(recipe),
      context_(context),
      data_(from_any(recipe->user_data()))
{
    set_type(TargetType::Archive);
}

void Recipe::set_type(TargetType type)
{
    recipe_->build_target().type = type;
}

void Recipe::set_working_directory(const std::string & dir)
{
    recipe_->set_working_directory(dir);
}

void Recipe::add(const std::string & dir, const std::string & pattern, const Flags & flags)
{
    model::GlobInfo info;
    info.dir = dir;
    info.pattern = pattern;
    info.mode = model::GlobInfo::Add;

    info.language = flags.get_or(Language::Undefined);
    info.type = flags.get_or(Type::Undefined);

    {
        auto p = flags.overwrite();
        if (p.second)
            info.overwrite = p.first;
    }

    {
        auto p = flags.propagation();
        if (p.second)
            info.propagation = p.first;
    }

    recipe_->add_globber(info);
}

void Recipe::remove(const std::string & dir, const std::string & pattern, const Flags & flags)
{
    model::GlobInfo info;
    info.dir = dir;
    info.pattern = pattern;
    info.mode = model::GlobInfo::Remove;

    recipe_->add_globber(info);
}

void Recipe::depends_on(const std::string & dependency)
{
    CHAI_MSS_BEGIN();

    auto s = log::scope("depends on", [&](auto & n) { n.attr("uri", recipe_->uri().string()).attr("dependency", dependency); });

    model::Uri uri;
    CHAI_MSS(model::Uri::recipe_uri(dependency, uri));
    CHAI_MSS(recipe_->add_dependency(uri));
}

void Recipe::library(const std::string & library, const Flags & flags)
{
    CHAI_MSS_BEGIN();
    CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

    auto lib = ingredient::KeyValue(library);
    lib.set_propagation(flags.get_or(Propagation::Public));
    lib.set_overwrite(flags.get_or(Overwrite::IfSame));

    recipe_->key_values().insert(LanguageTypePair(flags.get_or(Language::Binary), flags.get_or(Type::Library)), lib);
}

void Recipe::library_path(const std::string & path, const Flags & flags)
{
    CHAI_MSS_BEGIN();
    CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

    auto lib_path = ingredient::File(path, {});
    lib_path.set_propagation(flags.get_or(Propagation::Public));
    lib_path.set_overwrite(flags.get_or(Overwrite::Always));

    recipe_->files().insert(LanguageTypePair(flags.get_or(Language::Binary), flags.get_or(Type::LibraryPath)), lib_path);
}

void Recipe::include_path(const std::string & path, const Flags & flags)
{
    CHAI_MSS_BEGIN();
    CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

    auto inc_path = ingredient::File(path, {});

    inc_path.set_propagation(flags.get_or(Propagation::Public));
    inc_path.set_overwrite(flags.get_or(Overwrite::Always));

    recipe_->files().insert(LanguageTypePair(flags.get_or(Language::Undefined), flags.get_or(Type::IncludePath)), inc_path);
}

void Recipe::define(const std::string & name, const Flags & flags)
{
    CHAI_MSS_BEGIN();
    CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

    auto kv = ingredient::KeyValue(name);
    kv.set_propagation(flags.get_or(Propagation::Public));
    kv.set_overwrite(flags.get_or(Overwrite::IfSame));

    recipe_->key_values().insert(LanguageTypePair(flags.get_or(Language::Undefined), flags.get_or(Type::Define)), kv);
}

void Recipe::define(const std::string & name, const std::string & value, const Flags & flags)
{
    CHAI_MSS_BEGIN();
    CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

    auto kv = ingredient::KeyValue(name, value);
    kv.set_propagation(flags.get_or(Propagation::Public));
    kv.set_overwrite(flags.get_or(Overwrite::IfSame));

    recipe_->key_values().insert(LanguageTypePair(flags.get_or(Language::Undefined), flags.get_or(Type::Define)), kv);
}


} }
