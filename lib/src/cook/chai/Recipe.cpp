#include "cook/chai/Recipe.hpp"
#include "cook/chai/File.hpp"
#include "cook/chai/Context.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/chai/RaiiIngredient.hpp"
#include "cook/chai/mss.hpp"

namespace cook { namespace chai {

    Recipe::Recipe(model::Recipe * recipe, const Context *context)
    : recipe_(recipe),
    context_(context),
    data_(from_any(recipe->user_data()))
    {
    }

    bool Recipe::operator==(const Recipe & rhs) const
    {
        return uri() == rhs.uri();
    }
    bool Recipe::operator!=(const Recipe & rhs) const
    {
        return uri() != rhs.uri();
    }

    void Recipe::set_type(TargetType type)
    {
        recipe_->build_target().type = type;
    }

    TargetType Recipe::type() const
    {
        return recipe_->build_target().type;
    }

    void Recipe::set_working_directory(const std::string & dir)
    {
        recipe_->set_working_directory(dir);
    }

    std::string Recipe::working_directory() const
    {
        return recipe_->working_directory().string();
    }

    const model::Uri & Recipe::uri() const 
    { 
        return recipe_->uri(); 
    }

    void Recipe::add(const std::string & dir, const std::string & pattern, const Flags & flags, const GlobFunctor &  functor)
    {
        model::GlobInfo info;
        info.dir = dir;
        info.pattern = pattern;
        info.mode = model::GlobInfo::Add;
        info.language = flags.get_or(Language::Undefined);
        info.type = flags.get_or(Type::Undefined);

        // we should copy the context, it is a member variable and (*this) will be destroyed before this functor is called
        const Context * context = context_;

        if (functor)
        {
            info.filter_and_adaptor = [=](LanguageTypePair & ltp, ingredient::File & file)
            {
                File f(ltp, file, context);

                auto p = functor(f);
                if (!p)
                    return false;

                ltp = f.language_type_pair();
                file = f.element();
                return true;
            };
        }

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

    void Recipe::remove(const std::string & dir, const std::string & pattern, const Flags & flags, const GlobFunctor & functor)
    {
        model::GlobInfo info;
        info.dir = dir;
        info.pattern = pattern;
        info.mode = model::GlobInfo::Remove;
        info.language = flags.get_or(Language::Undefined);
        info.type = flags.get_or(Type::Undefined);

        const Context * context = context_;
        if (functor)
        {
            info.filter_and_adaptor = [=](LanguageTypePair & ltp, ingredient::File & file)
            {
                File f(ltp, file, context);

                auto p = functor(f);
                if (!p)
                    return false;

                ltp = f.language_type_pair();
                file = f.element();
                return true;
            };
        }

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

    void Recipe::depends_on(const model::Uri & dependency, const DepFileFilter & file_filter, const DepKeyValueFilter & key_value_filter)
    {
        CHAI_MSS_BEGIN();

        auto s = log::scope("depends on", [&](auto & n) { n.attr("uri", recipe_->uri().string()).attr("dependency", dependency); });

        CHAI_MSS_MSG(!dependency.path().empty(), Error, "The dependency cannot be empty");

        CHAI_MSS(recipe_->add_dependency(dependency, file_filter, key_value_filter));
    }

    void Recipe::library(const std::string & library, const Flags & flags)
    {
        CHAI_MSS_BEGIN();
        CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

        auto lib = ingredient::KeyValue(library);
        lib.set_content(Content::User);
        lib.set_propagation(flags.get_or(Propagation::Public));
        lib.set_overwrite(flags.get_or(Overwrite::Always));
        lib.set_owner(recipe_);

        recipe_->key_values().insert(LanguageTypePair(flags.get_or(Language::Binary), flags.get_or(Type::Library)), lib);
    }

    void Recipe::framework(const std::string & library, const Flags & flags)
    {
        CHAI_MSS_BEGIN();
        CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

        auto lib = ingredient::KeyValue(library);
        lib.set_content(Content::User);
        lib.set_propagation(flags.get_or(Propagation::Public));
        lib.set_overwrite(flags.get_or(Overwrite::Always));
        lib.set_owner(recipe_);

        recipe_->key_values().insert(LanguageTypePair(flags.get_or(Language::Binary), flags.get_or(Type::Framework)), lib);
    }
    void Recipe::library_path(const std::string & path, const Flags & flags)
    {
        CHAI_MSS_BEGIN();
        CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

        auto lib_path = ingredient::File(path, {});
        lib_path.set_content(Content::User);
        lib_path.set_propagation(flags.get_or(Propagation::Public));
        lib_path.set_overwrite(flags.get_or(Overwrite::Always));
        lib_path.set_owner(recipe_);

        recipe_->files().insert(LanguageTypePair(flags.get_or(Language::Binary), flags.get_or(Type::LibraryPath)), lib_path);
    }

    void Recipe::framework_path(const std::string & path, const Flags & flags)
    {
        CHAI_MSS_BEGIN();
        CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

        auto lib_path = ingredient::File(path, {});
        lib_path.set_content(Content::User);
        lib_path.set_propagation(flags.get_or(Propagation::Public));
        lib_path.set_overwrite(flags.get_or(Overwrite::Always));
        lib_path.set_owner(recipe_);

        recipe_->files().insert(LanguageTypePair(flags.get_or(Language::Binary), flags.get_or(Type::FrameworkPath)), lib_path);
    }
    void Recipe::include_path(const std::string & path, const Flags & flags)
    {
        CHAI_MSS_BEGIN();
        CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

        auto inc_path = ingredient::File(path, {});
        inc_path.set_content(Content::User);

        inc_path.set_propagation(flags.get_or(Propagation::Public));
        inc_path.set_overwrite(flags.get_or(Overwrite::Always));
        inc_path.set_owner(recipe_);

        recipe_->files().insert(LanguageTypePair(flags.get_or(Language::Undefined), flags.get_or(Type::IncludePath)), inc_path);
    }

    void Recipe::define(const std::string & name, const Flags & flags)
    {
        CHAI_MSS_BEGIN();
        CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

        auto kv = ingredient::KeyValue(name);
        kv.set_content(Content::User);
        kv.set_propagation(flags.get_or(Propagation::Public));
        kv.set_overwrite(flags.get_or(Overwrite::IfSame));
        kv.set_owner(recipe_);

        recipe_->key_values().insert(LanguageTypePair(flags.get_or(Language::Undefined), flags.get_or(Type::Define)), kv);
    }

    void Recipe::define(const std::string & name, const std::string & value, const Flags & flags)
    {
        CHAI_MSS_BEGIN();
        CHAI_MSS(flags.only({Flag::Propagation, Flag::Overwrite, Flag::Language, Flag::Type} ));

        auto kv = ingredient::KeyValue(name, value);
        kv.set_content(Content::User);
        kv.set_propagation(flags.get_or(Propagation::Public));
        kv.set_overwrite(flags.get_or(Overwrite::IfSame));
        kv.set_owner(recipe_);

        recipe_->key_values().insert(LanguageTypePair(flags.get_or(Language::Undefined), flags.get_or(Type::Define)), kv);
    }

    void Recipe::run(const std::string & command)
    {
        auto kv = ingredient::KeyValue(command, "");
        kv.set_content(Content::User);
        kv.set_propagation(Propagation::Private);
        kv.set_overwrite(Overwrite::Never);
        kv.set_owner(recipe_);
        recipe_->key_values().insert(LanguageTypePair(Language::Script, Type::Executable), kv);
    }

    bool Recipe::add_file(const std::string & dir, const std::string & rel, const Flags & flags)
    {
        CHAI_MSS_BEGIN();
        auto file = ingredient::File(dir, rel);
        file.set_content(Content::User);
        file.set_propagation(flags.get_or(Propagation::Public));
        file.set_overwrite(flags.get_or(Overwrite::IfSame));
        file.set_owner(recipe_);

        LanguageTypePair ltp(flags.get_or(Language::Undefined), flags.get_or(Type::Undefined));
        return recipe_->files().insert(ltp, file).second;
    }

    bool Recipe::add_key_value(const std::string & key, const Flags & flags)
    {
        CHAI_MSS_BEGIN();
        auto key_value = ingredient::KeyValue(key);
        key_value.set_content(Content::User);
        key_value.set_propagation(flags.get_or(Propagation::Public));
        key_value.set_overwrite(flags.get_or(Overwrite::IfSame));
        key_value.set_owner(recipe_);

        LanguageTypePair ltp(flags.get_or(Language::Undefined), flags.get_or(Type::Undefined));
        return recipe_->key_values().insert(ltp, key_value).second;
    }

    bool Recipe::add_key_value(const std::string & key, const std::string & value, const Flags & flags)
    {
        CHAI_MSS_BEGIN();
        auto key_value = ingredient::KeyValue(key, value);
        key_value.set_content(Content::User);
        key_value.set_propagation(flags.get_or(Propagation::Public));
        key_value.set_overwrite(flags.get_or(Overwrite::IfSame));
        key_value.set_owner(recipe_);

        LanguageTypePair ltp(flags.get_or(Language::Undefined), flags.get_or(Type::Undefined));
        return recipe_->key_values().insert(ltp, key_value).second;
    }

    void Recipe::each_file(const std::function<void (File &)> & functor)
    {
        const Context * context = context_;
        auto each_functor = [=](const LanguageTypePair & ltp, ingredient::File & file)
        {
            CHAI_MSS_BEGIN();

            LanguageTypePair ltp2 = ltp;
            RaiiFile f(ltp2, file, context);
            functor(f.chai);

            CHAI_MSS_MSG(ltp2 == ltp, Error, "Unable to change the language or the type");

            return true;
        };
        recipe_->files().each(each_functor);
    }

    void Recipe::each_key_value(const std::function<void (KeyValue &)> & functor)
    {
        const Context * context = context_;
        auto each_functor = [=](const LanguageTypePair & ltp, ingredient::KeyValue & key_value)
        {
            CHAI_MSS_BEGIN();

            LanguageTypePair ltp2 = ltp;
            RaiiKeyValue f(ltp2, key_value, context);
            functor(f.chai);
            
            CHAI_MSS_MSG(ltp2 == ltp, Error, "Unable to change the language or the type");
            
            return true;
        };
        recipe_->key_values().each(each_functor);

    }

    void Recipe::set_callback1(Hook hook, const ConfigCallback1 & callback)
    {
        recipe_->set_callback(hook, [=](model::Recipe & recipe) { callback(); });
    }

    void Recipe::set_callback2(Hook hook, const ConfigCallback2 & callback)
    {
        const Context * ctx = context_;
        recipe_->set_callback(hook, [=](model::Recipe & recipe) { callback(Recipe(&recipe, ctx)); });
    }

    std::string & Recipe::build_target_name()
    {
        return recipe_->build_target().name;
    }

    std::string Recipe::build_target_filename() const
    {
        return context_->toolchain().get_primary_target(*recipe_).string();
    }

} }
