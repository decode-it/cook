#include "cook/chai/module/Recipe.hpp"
#include "cook/chai/module/EnumHelper.hpp"
#include "cook/chai/module/RaiiIngredient.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/chai/File.hpp"
#include "cook/chai/KeyValue.hpp"
#include "cook/TargetType.hpp"
#include "cook/Hook.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai { namespace module {

    using DependencyFilter = std::function<bool (chaiscript::Boxed_Value vt)>;

    CREATE_WRAPPER_TYPE(TargetType);
    CREATE_WRAPPER_TYPE(Hook);

    namespace {


        TargetType make_user_defined(const wrapper::TargetType_t &, unsigned int val) 
        {
            return static_cast<TargetType>(static_cast<unsigned int>(TargetType::UserDefined) + val);
        }

        void filtered_depends_on(Recipe & recipe, const model::Uri & dep, const DependencyFilter & filter)
        {
            const Context * context = recipe.context();

            auto file_dep = [=](LanguageTypePair & ltp, ingredient::File & file)
            {
                RaiiFile f(ltp, file, context);
                return filter(as_boxed(f));
            };

            auto key_value_dep = [=](LanguageTypePair & ltp, ingredient::KeyValue & key_value)
            {
                RaiiKeyValue kv(ltp, key_value, context);
                return filter(as_boxed(kv));
            };

            recipe.depends_on(dep, file_dep, key_value_dep);
        }

    }
    
    gubg::chai::ModulePtr recipe()
    {
        auto ptr = gubg::chai::make_module();

        EXPOSE_TYPE(TargetType);
        EXPOSE_VALUE(TargetType, Undefined);
        EXPOSE_VALUE(TargetType, Object);
        EXPOSE_VALUE(TargetType, Archive);
        EXPOSE_VALUE(TargetType, SharedLibrary);
        EXPOSE_VALUE(TargetType, Executable);
        EXPOSE_VALUE(TargetType, Script);
        ptr->add(chaiscript::fun(make_user_defined), "UserDefined");

        EXPOSE_TYPE(Hook);
        EXPOSE_VALUE(Hook, Pre);
        EXPOSE_VALUE(Hook, Post);

        ptr->add(chaiscript::user_type<Recipe::ConfigCallback1>(), "ConfigurationCallback");
        ptr->add(chaiscript::user_type<Recipe::ConfigCallback2>(), "ConfigurationCallback");


        ptr->add(chaiscript::user_type<DependencyFilter>(), "DependencyFilter");
        ptr->add(chaiscript::user_type<Recipe>(), "Recipe");
        ptr->add(chaiscript::constructor<Recipe(const Recipe &)>(), "Recipe");
        ptr->add(chaiscript::fun(&Recipe::operator==), "==");
        ptr->add(chaiscript::fun(&Recipe::operator!=), "!=");
        ptr->add(chaiscript::fun(&Recipe::set_type), "set_type");
        ptr->add(chaiscript::fun(&Recipe::type), "type");
        ptr->add(chaiscript::fun(&Recipe::working_directory), "working_dir");
        

        // all the add overloads
        {
            auto add = [](Recipe & recipe, const std::string & dir, const std::string & rel) {
                recipe.add(dir, rel);
            };
            ptr->add(chaiscript::fun(add), "add");
        }
        {
            auto add = [](Recipe & recipe, const std::string & dir, const std::string & rel, const Flags & flags) {
                recipe.add(dir, rel, flags);
            };
            ptr->add(chaiscript::fun(add), "add");
        }
        {
            auto add = [](Recipe & recipe, const std::string & dir, const std::string & rel, const Recipe::GlobFunctor & functor) {
                recipe.add(dir, rel, Flags(), functor);
            };
            ptr->add(chaiscript::fun(add), "add");
        }
        {
            auto add = [](Recipe & recipe, const std::string & dir, const std::string & rel, const Flags & flags, const Recipe::GlobFunctor & functor) {
                recipe.add(dir, rel, flags, functor);
            };
            ptr->add(chaiscript::fun(add), "add");
        }

        // the remove overloads
        {
            auto remove = [](Recipe & recipe, const std::string & dir, const std::string & rel) {
                recipe.remove(dir, rel);
            };
            ptr->add(chaiscript::fun(remove), "remove");
        }
        {
            auto remove = [](Recipe & recipe, const std::string & dir, const std::string & rel, const Recipe::GlobFunctor & functor) {
                recipe.remove(dir, rel);
            };
            ptr->add(chaiscript::fun(remove), "remove");
        }

        ptr->add(chaiscript::fun([](Recipe & r, const std::string & n) { r.library(n); }), "library");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & n) { r.framework(n); }), "framework");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & p) { r.library_path(p); }), "library_path");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & p) { r.framework_path(p); }), "framework_path");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & p) { r.include_path(p); }), "include_path");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k) { r.define(k); }), "define");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k, const Flags & f) { r.define(k, f); }), "define");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k, const std::string & v) { r.define(k, v); }), "define");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k, const std::string & v, const Flags & f) { r.define(k, v, f); }), "define");
        
        ptr->add(chaiscript::fun(&Recipe::run), "run");
      
        ptr->add(chaiscript::fun(&Recipe::library), "library");
        ptr->add(chaiscript::fun(&Recipe::library_path), "library_path");
        ptr->add(chaiscript::fun(&Recipe::framework), "framework");
        ptr->add(chaiscript::fun(&Recipe::framework_path), "framework_path");
        ptr->add(chaiscript::fun(&Recipe::include_path), "include_path");
        ptr->add(chaiscript::fun(&Recipe::set_working_directory), "set_working_directory");
        ptr->add(chaiscript::fun(&Recipe::uri), "uri");
        ptr->add(chaiscript::fun(&Recipe::data), "data");

        ptr->add(chaiscript::fun([](Recipe & recipe, const model::Uri & dep) { recipe.depends_on(dep); }), "depends_on");
        ptr->add(chaiscript::fun(filtered_depends_on), "depends_on");

        ptr->add(chaiscript::fun([](Recipe & r, const std::string & d, const std::string & v) { return r.add_file(d, v); }), "add_file");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & d, const std::string & v, const Flags & f) { return r.add_file(d, v,f ); }), "add_file");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k) { return r.add_key_value(k); }), "add_key_value");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k, const Flags & f) { return r.add_key_value(k, f); }), "add_key_value");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k, const std::string & v) { return r.add_key_value(k, v); }), "add_key_value");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k, const std::string & v, const Flags & f) { return r.add_key_value(k, v, f); }), "add_key_value");

        ptr->add(chaiscript::fun(&Recipe::set_callback1), "set_config_callback");
        ptr->add(chaiscript::fun(&Recipe::set_callback2), "set_config_callback");

        ptr->add(chaiscript::fun(&Recipe::build_target_name), "name");
        ptr->add(chaiscript::fun(&Recipe::build_target_filename), "primary_target_filename");

        ptr->add(chaiscript::fun(&Recipe::each_file), "each_file");
        ptr->add(chaiscript::fun(&Recipe::each_key_value), "each_key_value");
        {
            auto lambda = [](Recipe & recipe, const std::function<void (chaiscript::Boxed_Value)> & functor)
            {
                recipe.each_file([=](File & f) { functor(chaiscript::var(&f)); });
                recipe.each_key_value([=](KeyValue & f) { functor(chaiscript::var(&f)); });
            };
        }
        
        return ptr;
    }

} } }
