#include "cook/chai/module/Recipe.hpp"
#include "cook/chai/module/EnumHelper.hpp"
#include "cook/chai/Recipe.hpp"
#include "cook/chai/File.hpp"
#include "cook/chai/KeyValue.hpp"
#include "cook/TargetType.hpp"
#include "gubg/chai/Module.hpp"

namespace cook { namespace chai { namespace module {

    using DependencyFilter = std::function<bool (chaiscript::Boxed_Value vt)>;

    CREATE_WRAPPER_TYPE(TargetType);

    namespace {

        template <typename SourceType, typename ChaiType>
        struct RaiiIngredient
        {

            RaiiIngredient(LanguageTypePair & ltp, SourceType & ingredient, const Context * context)
            : ltp_(ltp), ingredient_(ingredient), chai(ltp, ingredient, context)
            {
            }

            ~RaiiIngredient()
            {
                // update the flags back into the references
                ltp_ = chai.language_type_pair();
                chai.install_flags();

                // set the reference to the element
                ingredient_ = chai.element();
            }

            chaiscript::Boxed_Value as_boxed()
            {
                return chaiscript::var(&chai);
            }


            LanguageTypePair & ltp_;
            SourceType & ingredient_;
            ChaiType chai;
        };


        TargetType make_user_defined(const wrapper::TargetType_t &, unsigned int val) 
        {
            return static_cast<TargetType>(static_cast<unsigned int>(TargetType::UserDefined) + val);
        }

        void filtered_depends_on(Recipe & recipe, const model::Uri & dep, const DependencyFilter & filter)
        {
            const Context * context = recipe.context();

            auto file_dep = [=](LanguageTypePair & ltp, ingredient::File & file)
            {
                RaiiIngredient<ingredient::File, File> f(ltp, file, context);
                return filter(f.as_boxed());
            };

            auto key_value_dep = [=](LanguageTypePair & ltp, ingredient::KeyValue & key_value)
            {
                RaiiIngredient<ingredient::KeyValue, KeyValue> kv(ltp, key_value, context);
                return filter(kv.as_boxed());
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


        ptr->add(chaiscript::user_type<DependencyFilter>(), "DependencyFilter");
        ptr->add(chaiscript::user_type<Recipe>(), "Recipe");
        ptr->add(chaiscript::fun(&Recipe::set_type), "set_type");
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
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & p) { r.library_path(p); }), "library_path");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & p) { r.include_path(p); }), "include_path");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k) { r.define(k); }), "define");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k, const Flags & f) { r.define(k, f); }), "define");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k, const std::string & v) { r.define(k, v); }), "define");
        ptr->add(chaiscript::fun([](Recipe & r, const std::string & k, const std::string & v, const Flags & f) { r.define(k, v, f); }), "define");
        
        ptr->add(chaiscript::fun(&Recipe::run), "run");
      
        ptr->add(chaiscript::fun(&Recipe::library), "library");
        ptr->add(chaiscript::fun(&Recipe::library), "library_path");
        ptr->add(chaiscript::fun(&Recipe::library), "include_path");
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
        
        return ptr;
    }

} } }
