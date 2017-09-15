#include "cook/chai/Loader.hpp"
#include "cook/chai/Details.hpp"
#include "cook/structure/Recipe.hpp"
#include <functional>

namespace cook { namespace chai { 

bool Loader::load(structure::Book & root)
{
    MSS_BEGIN(bool);
    
    GlobalInfo info(options_, root);
    
    // initialize the engine
    auto & chai = info.engine.raw();
    chai.add(chaiscript::fun(&GlobalInfo::add_dir, &info),              "add_dir");
    chai.add(chaiscript::fun(&GlobalInfo::add_book, &info),             "add_book");
    chai.add(chaiscript::fun(&GlobalInfo::create_new_recipe, &info),    "add_recipe");
    chai.add(chaiscript::fun(&GlobalInfo::set_display_name, &info),     "display_name");
    chai.add(chaiscript::fun(&GlobalInfo::print, &info),                "print");
    
    chai.add(chaiscript::fun(&BookWrapper::add_dir),                    "add_dir");
    chai.add(chaiscript::fun(&BookWrapper::add_book),                   "add_book");
    chai.add(chaiscript::fun(&BookWrapper::create_new_recipe),          "add_recipe");
    chai.add(chaiscript::fun(&BookWrapper::set_display_name),           "display_name");
    chai.add(chaiscript::fun(&BookWrapper::print),                      "print");
    
    chai.add(chaiscript::fun(&RecipeWrapper::add_source),               "add_source");
    chai.add(chaiscript::fun(&RecipeWrapper::add_header),               "add_header");
    chai.add(chaiscript::fun(&RecipeWrapper::add_include_path),         "add_include_path");
    chai.add(chaiscript::fun(&RecipeWrapper::add_define_1),             "add_define");
    chai.add(chaiscript::fun(&RecipeWrapper::add_define_2),             "add_define");
    chai.add(chaiscript::fun(&RecipeWrapper::add_library_path),         "add_library_path");
    chai.add(chaiscript::fun(&RecipeWrapper::add_library),              "add_library");
    chai.add(chaiscript::fun(&RecipeWrapper::depends_on),               "depends_on");
    chai.add(chaiscript::fun(&RecipeWrapper::print),                    "print");
    chai.add(chaiscript::fun(&RecipeWrapper::set_display_name),         "display_name");
    chai.add(chaiscript::fun(&RecipeWrapper::set_target_type),          "target_type");
    
    chai.add_global_const(chaiscript::const_var(structure::TargetType::Executable), "executable");
    chai.add_global_const(chaiscript::const_var(structure::TargetType::StaticLibrary), "static_library");
//     chai.add(chaiscript::user_type<structure::TargetType::TestEnum>(), "TestEnum");
//     chai.add_global_const(chaiscript::const_var<structure::TargetType::StaticLibrary>, "static_library");
    
    //         chai.add(chaiscript::fun(add_book, &info), "add_book");
    
    // process an empty dir request
    MSS(info.add_dir(""));
    
    
    MSS_END();
}

} }
