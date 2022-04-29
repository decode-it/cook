#include "cook/generator/CMake.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/Context.hpp"
#include "cook/util/File.hpp"
#include "gubg/stream.hpp"
#include "gubg/string/escape.hpp"
#include <list>
#include <cassert>

namespace cook { namespace generator { namespace {

    std::list<const model::Recipe *> top_order(const std::list<model::Recipe*> & all_recipes, const std::unordered_set<const model::Recipe *> & recipes)
    {
        std::list<const model::Recipe *> result;
        for(auto it = all_recipes.begin(); it != all_recipes.end(); ++it)
            if(recipes.count(*it) > 0)
                result.push_front(*it);

        return result;
    }

    using RecipeInfoMap = CMake::RecipeInfoMap;
    using CMakeType = CMake::CMakeType;

    CMakeType proposed_type(const model::Recipe & recipe)
    {
        switch(recipe.build_target().type)
        {
            case TargetType::Object:
            case TargetType::Undefined:
                return CMakeType::Object;
            case TargetType::Archive:
                return CMakeType::StaticLibrary;
            case TargetType::SharedLibrary:
                return CMakeType::SharedLibrary;
            case TargetType::Plugin:
                return CMakeType::Module;
            case TargetType::Executable:
                return CMakeType::Executable;
            default:
                return CMakeType::Skip;
        }
    }

    bool contains_sources(const model::Recipe & recipe, const RecipeInfoMap & map)
    {
        // check if the recipe has source files
        bool has_sources = false;
        {
            auto check_src = [&](const LanguageTypePair & ltp, const auto & ) 
            {
                has_sources = has_sources || (ltp.type == Type::Source);
                return true;
            };
            recipe.files().each(check_src);
        }

        // if we have, then trivial
        if (has_sources)
            return true;

        switch(proposed_type(recipe))
        {
            case CMakeType::Executable:
            case CMakeType::SharedLibrary:
            case CMakeType::Module:
            case CMakeType::StaticLibrary:
                break;

            default:
                return false;
        }

        for(const auto * dep : recipe.dependencies())
        {
            auto it = map.find(dep);
            assert(it != map.end());

            if (it->second.type == CMakeType::Object)
                return true;
        }

        return false;
    }


    Result construct_recipe_map(RecipeInfoMap & map, const Context & context)
    {
        MSS_BEGIN(Result);

        for (const model::Recipe * recipe : context.menu().topological_order_recipes())
        {
            bool has_object_deps = false;
            for(const auto * dep : recipe->dependencies())
            {
                auto it = map.find(dep);
                MSS(it != map.end());
                const CMake::RecipeInfo & dep_info = it->second;

                has_object_deps = has_object_deps || !dep_info.object_recipes_.empty();
            }


            bool has_sources = contains_sources(*recipe, map);
            CMake::RecipeInfo & info  = map[recipe];
            info.links_away_objects = info.links_away_libraries = false;

            switch(recipe->build_target().type)
            {
                case TargetType::Executable:
                    if (!has_sources)
                    {
                        MSS_RC << MESSAGE(Warning, "CMake: skipping " << recipe->uri() << ", executable without sources");
                        info.type = CMakeType::Skip;
                    }
                    else
                    {
                        info.type = CMakeType::Executable;
                        info.links_away_objects = info.links_away_libraries = true;
                    }
                    break;

                case TargetType::SharedLibrary:
                    if (!has_sources)
                    {
                        MSS_RC << MESSAGE(Warning, "CMake: skipping " << recipe->uri() << ", shared library without sources");
                        info.type = CMakeType::Skip;
                    }
                    else
                    {
                        info.type = CMakeType::SharedLibrary;
                        info.links_away_objects = info.links_away_libraries = true;

                    }
                    break;
                case TargetType::Plugin:
                    if (!has_sources)
                    {
                        MSS_RC << MESSAGE(Warning, "CMake: skipping " << recipe->uri() << ", shared library without sources");
                        info.type = CMakeType::Skip;
                    }
                    else
                    {
                        info.type = CMakeType::Module;
                        info.links_away_objects = info.links_away_libraries = true;

                    }
                    break;

                case TargetType::Archive:
                    
                    info.type = has_sources || has_object_deps ? CMakeType::StaticLibrary : CMakeType::Interface;
                    info.links_away_objects = true;
                    break;

                case TargetType::Undefined:
                case TargetType::Object:
                    info.type = has_sources ? CMakeType::Object : CMakeType::Interface;
                    break;

                default:
                    info.type = CMakeType::Skip;
                    MSS_RC << MESSAGE(Warning, "No CMake translation possible for recipe " << recipe->uri() << " with type " << recipe->build_target().type);
            }

            if (info.type == CMakeType::Object)
            {
                info.object_recipes_.insert(recipe);
            } else if (info.links_away_objects || info.links_away_libraries) {
                info.link_targets_.insert(recipe);
            }

            // perform the propagation
            for(const auto * dep : recipe->dependencies())
            {
                auto it = map.find(dep);
                MSS(it != map.end());
                const CMake::RecipeInfo & dep_info = it->second;

                if (!dep_info.links_away_libraries)
                    info.link_targets_.insert(dep_info.link_targets_.begin(), dep_info.link_targets_.end());
                if (!dep_info.links_away_objects)
                    info.object_recipes_.insert(dep_info.object_recipes_.begin(), dep_info.object_recipes_.end());
            }
        }

        MSS_END();
    }

}

CMake::CMake()
    : context_(nullptr)
{
    set_option("./");
}

Result CMake::set_option(const std::string & option)
{
    MSS_BEGIN(Result);
    output_path_ = option;
    set_filename(gubg::string::escape_cmake(gubg::filesystem::combine(output_path_, default_filename()).string()));
    MSS_END();
}

bool CMake::can_process(const Context & context) const
{
    return context.menu().is_valid();
}

Result CMake::process(const Context & context)
{
    MSS_BEGIN(Result);

    context_ = &context;

    auto get_output_dir = [&](const std::filesystem::path & recipe_local_dir)
    {
        std::filesystem::path adj = util::get_from_to_path(std::filesystem::current_path(), recipe_local_dir);
        std::filesystem::path res = output_path_ / adj;

        return res.empty() ? "./" : res;
    };

    std::list<std::pair<std::filesystem::path, std::ofstream>> streams;
    auto get_output_stream = [&](const std::filesystem::path & wd, std::ofstream *& str, bool & is_new)
    {
        MSS_BEGIN(bool);
        std::filesystem::path actual_path = get_output_dir(wd);
        is_new = false;

        // an existing ?
        for(auto & p : streams)
        {
            //We swallow any error due to underlying OS issues, hence the std::error_code
            std::error_code ec;
            if (std::filesystem::equivalent(p.first, actual_path, ec))
            {
                str = &p.second;
                MSS_RETURN_OK();
            }
        }

        is_new = true;

        // a new
        streams.push_back(std::make_pair(actual_path, std::ofstream()));
        str = &streams.back().second;

        MSS(util::open_file(actual_path / default_filename(), *str));

        MSS_END();
    };


    std::ofstream * root = nullptr;
    {
        bool is_new = false;
        MSS(get_output_stream(std::filesystem::current_path(), root, is_new));
        MSS(!!root);
    }

    std::ofstream & ofs = *root;
    const auto & recipe_list = context.menu().topological_order_recipes();
    ofs << "cmake_minimum_required (VERSION 3.12)" << std::endl;

    //See `cmake --help-policy CMP0076`
    //Reduces warning probably generated because relative paths are used
    //`target_sources()` for targets where the `SOURCE_DIR` != `CMAKE_CURRENT_SOURCE_DIR`
    ofs << "cmake_policy(SET CMP0076 NEW)" << std::endl;

    {
        std::string name = context.project_name();
        if (name.empty())
        {
            auto cp = std::filesystem::current_path();
            auto it = cp.end();
            if (!cp.empty())
                name = (--it)->string();
            else
                name = "undefined";
        }
        ofs << "project (" << name << ")" << std::endl;
    }

    auto set_property = [&](const std::string & key, const std::string & cmake_key)
    {
        std::list<std::string> values = context.toolchain().config_values(key);

        for(auto it = values.begin(); it != values.end(); ++it)
            ofs << "set(" << cmake_key << " " << *it << ")" << std::endl;
    };

    set_property("c++.std", "CMAKE_CXX_STANDARD");
    set_property("c.std", "CMAKE_C_STANDARD");

    if (context.toolchain().has_config("position_independent_code", "true"))
        ofs << "set(CMAKE_POSITION_INDEPENDENT_CODE ON)" << std::endl;

    recipe_map_.clear();
    MSS(construct_recipe_map(recipe_map_, context));
    for(model::Recipe * recipe : recipe_list)
    {
        auto it = recipe_map_.find(recipe);
        MSS(it != recipe_map_.end());
        const RecipeInfo & info = it->second;
        if (info.type == CMakeType::Skip)
            continue;

        std::ofstream * str = nullptr;
        bool is_new = false;
        MSS(get_output_stream(recipe->working_directory(), str, is_new));
        MSS(!!str);

        if (is_new)
        {
            std::filesystem::path p = util::get_from_to_path(std::filesystem::current_path(), recipe->working_directory());
            ofs << "add_subdirectory(" << gubg::string::escape_cmake(p.string()) << ")" << std::endl;
        }

        std::filesystem::path output_dir = get_output_dir(recipe->working_directory());
        std::filesystem::path output_to_source = util::get_from_to_path(output_dir, *recipe);

        switch(info.type)
        {
            case CMakeType::Object:
                MSS(add_object_library_(*str, recipe, context, output_to_source));
                break;
            case CMakeType::StaticLibrary:
                MSS(add_static_library_(*str, recipe, context, output_to_source, top_order(recipe_list, info.object_recipes_)));
                break;
            case CMakeType::SharedLibrary:
                MSS(add_shared_library_(*str, recipe, context, output_to_source, top_order(recipe_list, info.object_recipes_), top_order(recipe_list, info.link_targets_), CMakeType::SharedLibrary));
                break;
            case CMakeType::Module:
                MSS(add_shared_library_(*str, recipe, context, output_to_source, top_order(recipe_list, info.object_recipes_), top_order(recipe_list, info.link_targets_), CMakeType::Module));
                break;
            case CMakeType::Executable:
                MSS(add_executable_(*str, recipe, context, output_to_source, top_order(recipe_list, info.object_recipes_), top_order(recipe_list, info.link_targets_)));
                break;
            case CMakeType::Interface:
                MSS(add_interface_library_(*str, recipe, context, output_to_source));
                break;

            default:
                break;
        }
    }

    context_ = nullptr;
    MSS_END();
}

std::string CMake::default_filename() const
{
    return "CMakeLists.txt";
}

Result CMake::add_object_library_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source)
{
    MSS_BEGIN(Result);

    str << "# " << recipe->uri() << std::endl;

    str << "add_library(" << recipe_name_(recipe) << " OBJECT" << std::endl;
    add_source_and_header_(str, recipe, false, false, RecipeList(), output_to_source);
    str << ")" << std::endl;

    MSS(set_source_files_properties_(str, recipe, output_to_source));

    MSS(set_target_properties_(str, recipe, "PRIVATE", output_to_source));

    str << "# " << recipe->uri() << std::endl << std::endl;

    MSS_END();
}

Result CMake::add_static_library_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source, const RecipeList & objects)
{
    MSS_BEGIN(Result);

    str << "# " << recipe->uri() << std::endl;

    str << "add_library(" << recipe_name_(recipe) << " STATIC" << std::endl;
    add_source_and_header_(str, recipe, false, false, objects, output_to_source);
    str << ")" << std::endl;

    MSS(set_source_files_properties_(str, recipe, output_to_source));

    MSS(set_target_properties_(str, recipe, "PRIVATE", output_to_source));

    str << "# " << recipe->uri() << std::endl << std::endl;
    MSS_END();
}

Result CMake::add_shared_library_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source, const RecipeList & objects, const RecipeList & links, CMakeType type)
{
    MSS_BEGIN(Result);

    str << "# " << recipe->uri() << std::endl;
    set_link_paths_(str, recipe, output_to_source);

    str << "add_library(" << recipe_name_(recipe) << " " << (type == CMakeType::Module ? "MODULE" : "SHARED") << std::endl;
    const auto add_bundle_files = type == CMakeType::Module;
    add_source_and_header_(str, recipe, true, add_bundle_files, objects, output_to_source);
    str << ")" << std::endl;

    MSS(set_source_files_properties_(str, recipe, output_to_source));

    MSS(set_target_properties_(str, recipe, "PRIVATE", output_to_source));

    MSS(set_link_libraries(str, recipe, links, "PRIVATE", output_to_source));

    MSS(set_bundle_properties_(str, recipe, output_to_source));

    str << "# " << recipe->uri() << std::endl << std::endl;
    MSS_END();
}

Result CMake::add_executable_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source, const RecipeList & objects, const RecipeList & links)
{
    MSS_BEGIN(Result);

    str << "# " << recipe->uri() << std::endl;
    set_link_paths_(str, recipe, output_to_source);

    str << "add_executable(" << recipe_name_(recipe) << std::endl;
    add_source_and_header_(str, recipe, false, true, objects, output_to_source);
    str << ")" << std::endl;

    MSS(set_source_files_properties_(str, recipe, output_to_source));

    MSS(set_target_properties_(str, recipe, "PRIVATE", output_to_source));

    MSS(set_link_libraries(str, recipe, links, "PRIVATE", output_to_source));

    MSS(set_bundle_properties_(str, recipe, output_to_source));

    str << "# " << recipe->uri() << std::endl << std::endl;
    MSS_END();
}

Result CMake::add_interface_library_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source)
{
    MSS_BEGIN(Result);
    str << "# " << recipe->uri() << std::endl;

    str << "add_library(" << recipe_name_(recipe) << " INTERFACE)" << std::endl;

    str << "target_sources(" << recipe_name_(recipe) << " INTERFACE" << std::endl;
    auto add_hdr = [&](const LanguageTypePair & ltp, const ingredient::File & file)
    {
        if (ltp.type == Type::Header)
            str << "  " << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.key()).string()) << std::endl;
        return true;
    };
    recipe->files().each(add_hdr);
    str << ")" << std::endl;

    MSS(set_target_properties_(str, recipe, "INTERFACE", output_to_source));


    str << "# " << recipe->uri() << std::endl << std::endl;
    MSS_END();
}

std::string CMake::recipe_name_(const model::Recipe * recipe) const
{
    model::Uri uri = recipe->uri().as_relative();
    return uri.string('_');
}

Result CMake::add_source_and_header_(std::ostream & ofs, model::Recipe * recipe, bool add_exports, bool add_bundle_files, const RecipeList & possible_object_dependencies,  const std::filesystem::path & output_to_source) const
{
    MSS_BEGIN(Result);
    auto add_files = [&](const LanguageTypePair & ltp, const ingredient::File & file)
    {
        switch(ltp.type)
        {
            case Type::Source:
                ofs << "  " << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.key()).string()) << std::endl;
                break;

            case Type::Header:
                ofs << "  " << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.key()).string()) << std::endl;
                break;

            case Type::Export:
                if (add_exports)
                    ofs << "  " << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.key()).string()) << std::endl;
                break;

            case Type::Bundle:
                if (add_bundle_files)
                    //This is necessary for OSX bundles, maybe this should be disabled for other platforms
                    ofs << "  " << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.key()).string()) << std::endl;
                break;

            default:
                break;
        }
        return true;
    };

    MSS(recipe->files().each(add_files));


    for(const model::Recipe * dep : possible_object_dependencies)
    {
        if (dep == recipe)
            break;

        auto it = recipe_map_.find(dep);
        if(it != recipe_map_.end() && it->second.type == CMakeType::Object)
            ofs << "  $<TARGET_OBJECTS:" << recipe_name_(dep) << ">" << std::endl; 
    }

    MSS_END();
}

Result CMake::set_source_files_properties_(std::ostream & ofs, model::Recipe * recipe, const std::filesystem::path &output_to_source) const
{
    MSS_BEGIN(Result);

    auto add_files = [&](const LanguageTypePair & ltp, const ingredient::File & file)
    {
        std::vector<std::pair<std::string, std::string>> properties;

        switch(ltp.type)
        {
            case Type::Header: properties.emplace_back("HEADER_FILE_ONLY", "TRUE"); break;
            case Type::Bundle: properties.emplace_back("MACOSX_PACKAGE_LOCATION", "\"Resources\""); break;

            default: break;
        }

        if (!properties.empty())
        {
            ofs << "set_source_files_properties(" << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.key()).string()) << " PROPERTIES";
            for (const auto &property: properties)
                ofs << " " << property.first << " " << property.second;
            ofs << ")" << std::endl;
        }

        return true;
    };
    MSS(recipe->files().each(add_files));

    MSS_END();
}


namespace  {

    template <typename OpenFunction, typename Container>
    void write_elements_(std::ostream & ofs, OpenFunction && open_function, const Container & elements, bool new_line = true)
    {
        if (elements.empty())
            return;

        open_function(ofs);
        if (new_line)
            ofs << std::endl;

        if (new_line)
            for(const std::string & line : elements)
                ofs << "  " << line << std::endl;
        else
            for(const std::string & line : elements)
                ofs << " " << line;

        ofs << ")" << std::endl;
    }

}

void CMake::set_link_paths_(std::ostream & oss, model::Recipe * recipe, const std::filesystem::path & output_to_source) const
{
    std::list<std::string> link_directories;
    auto add_lib = [&](const LanguageTypePair & ltp, const ingredient::File & file)
    {
        if (ltp.type == Type::LibraryPath)
            link_directories.push_back(gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.dir()).string()));

        return true;
    };
    recipe->files().each(add_lib);

    write_elements_(oss, [](auto & os) { os << "link_directories("; }, link_directories, false);
}

Result CMake::set_link_libraries(std::ostream & ofs, model::Recipe * recipe, const RecipeList & dependencies, const std::string & keyword, const std::filesystem::path & output_to_source) const
{
    MSS_BEGIN(Result);

    const std::string & name = recipe_name_(recipe);

    std::list<std::string> link_libraries;
    // add the dependencies on the recipes
    for (const model::Recipe * dep : dependencies)
    {
        if (dep == recipe)
            continue;

        // can cmake handle this ?
        auto it = recipe_map_.find(dep);
        if (it == recipe_map_.end() || !it->second.links_away_objects)
            continue;

        // do we have a to this library 
        {
            bool found = false;
            auto find_lib = [&](const LanguageTypePair & ltp, const ingredient::KeyValue & kv)
            {
                // is this the lib generated by the dependency
                if (ltp.type == Type::Library && kv.owner() == dep && kv.content() == Content::Generated)
                    found = true;

                return true;
            };

            recipe->key_values().each(find_lib);
            if (found)
                link_libraries.push_back(recipe_name_(dep));
        }
    }

    for(const ingredient::KeyValue & lib : recipe->key_values().range(LanguageTypePair(Language::Binary, Type::Library)))
    {
        // only the ones added by the user
        if (!is_internal_generated(lib.content()))
            link_libraries.push_back(gubg::stream([&](auto & os) { os << lib.key(); } ));
    }


    {
        auto el = context_->toolchain().element(process::toolchain::Element::Link, Language::Binary, TargetType::Executable);
        MSS(!!el);
        // add the frameworks
        {
            auto itf = el->translator_map().find(process::toolchain::Part::Framework);
            MSS(itf != el->translator_map().end());

            auto add_f = [&](const LanguageTypePair & ltp, const ingredient::KeyValue & key_value)
            {
                if (ltp.type == Type::Framework)
                {
                    const auto & str = gubg::string::escape_cmake(itf->second(key_value.key(), ""));
                    link_libraries.push_back(str);
                }
                return true;
            };
            recipe->key_values().each(add_f);
        }

        // add the framework paths
        {
            auto itfp = el->translator_map().find(process::toolchain::Part::FrameworkPath);
            MSS(itfp != el->translator_map().end());
            std::ostringstream os;

            auto add_fp = [&](const LanguageTypePair & ltp, const ingredient::File & file)
            {
                if (ltp.type == Type::FrameworkPath)
                {
                    const auto & str = gubg::string::escape_cmake(itfp->second(gubg::filesystem::combine(output_to_source, file.dir()).string(), ""));
                    link_libraries.push_back(str);
                }
                return true;
            };
            recipe->files().each(add_fp);
        }
    }

    write_elements_(ofs, [&](auto & os) { os << "target_link_libraries(" << name << " " << keyword; }, link_libraries, false);       


    MSS_END();
}

bool CMake::set_target_properties_(std::ostream & ofs, model::Recipe * recipe, const std::string & keyword, const std::filesystem::path & output_to_source) const
{
    MSS_BEGIN(bool);

    const std::string & name = recipe_name_(recipe);

    // the include directories
    {
        std::set<std::string> include_dirs;
        auto add_include = [&](const LanguageTypePair & ltp, const ingredient::File & file)
        {
            if (ltp.type == Type::IncludePath)
            {
                include_dirs.insert(gubg::stream([&](auto & os) { os << keyword << " " << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.key()).string()); } ));
            }
            return true;
        };
        recipe->files().each(add_include);
        write_elements_(ofs, [&](auto & os) { os << "target_include_directories(" << name; }, include_dirs, true);
    }


    // the compile definitions
    {
        std::set<std::string> definitions;
        auto add_define = [&](const LanguageTypePair & ltp, const ingredient::KeyValue & key_value)
        {
            if (ltp.type == Type::Define)
                definitions.insert(key_value.to_string());
            return true;
        };
        recipe->key_values().each(add_define);

        write_elements_(ofs, [&](auto & os) { os << "target_compile_definitions(" << name << " " << keyword; }, definitions);
    }

    // the compile options
    {
        std::set<std::string> options;
        auto el = context_->toolchain().element(process::toolchain::Element::Compile, Language::C, TargetType::Object);
        MSS(!!el);

        auto it = el->translator_map().find(process::toolchain::Part::ForceInclude);
        MSS(it != el->translator_map().end());

        auto add_fi = [&](const LanguageTypePair &  ltp, const ingredient::File & file)
        {
            if (ltp.type == Type::ForceInclude)
            {
                const auto & str = gubg::string::escape_cmake(it->second(file.rel().string(), ""));
                
                std::ostringstream oss;
                oss << "\"SHELL:" << gubg::string::dequote(str) << "\"";
                options.insert(oss.str());
            }
            return true;
        };
        recipe->files().each(add_fi);

        write_elements_(ofs, [&](auto & os) { os << "target_compile_options(" << name << " " << keyword; }, options);
    }

    // the dependencies
    {
        std::list<std::string> dependencies;
        for(model::Recipe * dep : recipe->dependencies())
        {
            auto it = recipe_map_.find(dep);
            if (it != recipe_map_.end() && it->second.type != CMakeType::Skip)
                dependencies.push_back(recipe_name_(dep));
        }

        write_elements_(ofs, [&](auto & os) { os << "add_dependencies(" << name; }, dependencies, false);
    }

    MSS_END();
}

Result CMake::set_bundle_properties_(std::ostream & ofs, model::Recipe * recipe, const std::filesystem::path &output_to_source) const
{
    MSS_BEGIN(Result);

    const std::string & name = recipe_name_(recipe);

    bool create_bundle = false;
    std::optional<std::string> bundle_extension;
    std::optional<std::string> bundle_identifier;
    std::optional<std::string> bundle_name;
    std::optional<std::string> bundle_info;
    std::optional<std::string> bundle_version;
    std::optional<std::string> bundle_long_version;
    auto find_bundle_parameters = [&](const LanguageTypePair &ltp, const ingredient::KeyValue &kv){
        if (ltp.type == Type::Bundle)
        {
            if (kv.key() == "bundle") { create_bundle = (kv.value() == "true"); }
            else if (kv.key() == "extension") { bundle_extension = kv.value(); }
            else if (kv.key() == "identifier") { bundle_identifier = kv.value(); }
            else if (kv.key() == "name") { bundle_name = kv.value(); }
            else if (kv.key() == "info") { bundle_info = kv.value(); }
            else if (kv.key() == "version") { bundle_version = kv.value(); }
            else if (kv.key() == "long_version") { bundle_long_version = kv.value(); }
            else { MSS_RC << MESSAGE(Warning, "CMake: unknown Bundle setting `" << kv.key() << "`"); return false; }
        }
        return true;
    };
    recipe->key_values().each(find_bundle_parameters);

    std::optional<std::string> info_plist;
    std::optional<std::string> bundle_icon;
    auto find_bundle_files = [&](const LanguageTypePair &ltp, const ingredient::File &file){
        if (ltp.type == Type::Bundle)
        {
            if (ltp.language == Language::PropertyList)
            {
                if (info_plist)
                    MSS_RC << MESSAGE(Warning, "CMake: skipping property list file `" << file.key() << "`, already found `" << *info_plist << "`");
                else
                    info_plist = file.key();
            }
            else if (ltp.language == Language::Icon)
            {
                if (bundle_icon)
                    MSS_RC << MESSAGE(Warning, "CMake: skipping property icon `" << file.key() << "`, already found `" << *bundle_icon << "`");
                else
                    bundle_icon = file.rel().filename().string();
            }
        }
        return true;
    };
    recipe->files().each(find_bundle_files);

    //We only create a bundle if an Info.plist was set
    if (create_bundle)
    {
        const std::string resources_varname = "BUNDLE_RESOURCE_FILES";
        {
            ofs << "set(" << resources_varname << std::endl;
            auto add_resource_files = [&](const LanguageTypePair &ltp, const ingredient::File &file){
                if (ltp.type == Type::Bundle)
                    ofs << "    " << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.key()).string()) << std::endl;
                return true;
            };
            recipe->files().each(add_resource_files);
            ofs << ")" << std::endl;
        }

        ofs << "set_target_properties(" << name << " PROPERTIES" << std::endl;
        ofs << "    BUNDLE TRUE" << std::endl;
        if (recipe->build_target().type == TargetType::Executable)
            ofs << "    MACOSX_BUNDLE TRUE" << std::endl;
        if (bundle_identifier)
            ofs << "    MACOSX_BUNDLE_IDENTIFIER " << *bundle_identifier << std::endl;
        if (bundle_extension)
            ofs << "    BUNDLE_EXTENSION " << *bundle_extension << std::endl;
        if (bundle_icon)
            ofs << "    MACOSX_BUNDLE_ICON_FILE " << *bundle_icon << std::endl;
        if (bundle_name)
            ofs << "    MACOSX_BUNDLE_BUNDLE_NAME \"" << *bundle_name << "\"" << std::endl;
        if (bundle_info)
            ofs << "    MACOSX_BUNDLE_INFO_STRING \"" << *bundle_info << "\"" << std::endl;
        if (bundle_version)
        {
            ofs << "    MACOSX_BUNDLE_BUNDLE_VERSION \"" << *bundle_version << "\"" << std::endl;
            ofs << "    MACOSX_BUNDLE_SHORT_VERSION_STRING \"" << *bundle_version << "\"" << std::endl;
        }
        if (bundle_long_version)
            ofs << "    MACOSX_BUNDLE_LONG_VERSION_STRING \"" << *bundle_long_version << "\"" << std::endl;
        if (info_plist)
            ofs << "    MACOSX_BUNDLE_INFO_PLIST " << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, *info_plist).string()) << std::endl;
        ofs << "    RESOURCE \"${" << resources_varname << "}\"" << std::endl;
        ofs << ")" << std::endl;
    }

    MSS_END();
}

} }


