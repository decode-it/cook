#include "cook/generator/CMake.hpp"
#include "cook/process/toolchain/Manager.hpp"
#include "cook/Context.hpp"
#include "cook/util/File.hpp"
#include "gubg/stream.hpp"
#include "gubg/string/escape.hpp"
#include <list>

namespace cook { namespace generator {

    CMake::CMake()
    {
        set_option("");
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

    Result CMake::construct_recipe_map_(const Context & context)
    {
        MSS_BEGIN(Result);

        recipe_map_.clear();
        for(const model::Recipe * recipe : context.menu().topological_order_recipes())
        {
            CMakeType type;

            switch(recipe->build_target().type)
            {
                case TargetType::Executable:
                    {
                        bool has_sources = contains_sources_(*recipe, CMakeType::Executable);
                        if (!has_sources)
                        {
                            MSS_RC << MESSAGE(Warning, "CMake: skipping " << recipe->uri() << ", executable without sources");
                            type = CMakeType::Skip;
                        }
                        else
                        {
                            type = CMakeType::Executable;
                        }
                    }
                    break;

                case TargetType::SharedLibrary:
                    {
                        bool has_sources = contains_sources_(*recipe, CMakeType::SharedLibrary);
                        type = has_sources ? CMakeType::SharedLibrary : CMakeType::Skip;
                    }
                    break;

                case TargetType::Archive:
                    {
                        bool has_sources = contains_sources_(*recipe, CMakeType::StaticLibrary);
                        type = has_sources ? CMakeType::StaticLibrary : CMakeType::Interface;
                    }
                    break;

                case TargetType::Undefined:
                case TargetType::Object:
                    {
                        bool has_sources = contains_sources_(*recipe, CMakeType::Object);
                        type = has_sources ? CMakeType::Object : CMakeType::Interface;
                    }
                    break;

                default:
                    type = CMakeType::Skip;
                    MSS_RC << MESSAGE(Warning, "No CMake translation possible for recipe " << recipe->uri() << " with type " << recipe->build_target().type);
            }

            if (type  != CMakeType::Skip)
                recipe_map_[recipe] = type;
        }

        MSS_END();
    }

    Result CMake::process(const Context & context)
    {
        MSS_BEGIN(Result);

        std::ofstream ofs;
        MSS(open_output_stream(context, ofs));

        std::list<std::pair<std::filesystem::path, std::ofstream>> streams;
        std::filesystem::path root_path = gubg::filesystem::normalize(std::filesystem::current_path());

        auto get_output_stream = [&](const std::filesystem::path & wd, std::ofstream *& str, bool & is_new)
        {
            MSS_BEGIN(bool);

            std::filesystem::path cur = gubg::filesystem::combine(root_path, wd);

            is_new = false;

            // the root
            if (std::filesystem::equivalent(cur, root_path))
            {
                str = &ofs;
                MSS_RETURN_OK();
            }

            // an existing ?
            for(auto & p : streams)
                if (std::filesystem::equivalent(p.first, cur))
                {
                    str = &p.second;
                    MSS_RETURN_OK();
                }

            is_new = true;

            // a new
            streams.push_back(std::make_pair(cur, std::ofstream()));
            str = &streams.back().second;

            MSS(util::open_file(output_path_ / wd / default_filename(), *str));
            MSS_END();
        };

        MSS(construct_recipe_map_(context));

        const auto & recipe_list = context.menu().topological_order_recipes();
        ofs << "cmake_minimum_required (VERSION 3.1)" << std::endl;
        ofs << "project (" << context.project_name() << ")" << std::endl;

        auto set_property = [&](const std::string & key, const std::string & cmake_key)
        {
            std::list<std::string> values = context.toolchain().config_values(key);

            for(auto it = values.begin(); it != values.end(); ++it)
                ofs << "set(" << cmake_key << " " << *it << ")" << std::endl;
        };

        set_property("c++.std", "CMAKE_CXX_STANDARD");
        set_property("c.std", "CMAKE_C_STANDARD");

        for(model::Recipe * recipe : recipe_list)
        {
            auto it = recipe_map_.find(recipe);
            if (it == recipe_map_.end())
                continue;

            auto type = it->second;

            std::ofstream * str = nullptr;
            bool is_new = false;
            MSS(get_output_stream(recipe->working_directory(), str, is_new));
            MSS(!!str);

            if (is_new)
                ofs << "add_subdirectory(" << gubg::filesystem::normalize(recipe->working_directory()) << ")" << std::endl;


            std::filesystem::path output_to_source = gubg::filesystem::get_relative_to(output_path_ / recipe->working_directory(), recipe->working_directory());

            switch(type)
            {
                case CMakeType::Object:
                    MSS(add_object_library_(*str, recipe, context, output_to_source));
                    break;
                case CMakeType::StaticLibrary:
                    MSS(add_static_library_(*str, recipe, context, output_to_source));
                    break;
                case CMakeType::SharedLibrary:
                    MSS(add_shared_library_(*str, recipe, context, output_to_source));
                    break;
                case CMakeType::Executable:
                    MSS(add_executable_(*str, recipe, context, output_to_source));
                    break;
                case CMakeType::Interface:
                    MSS(add_interface_library_(*str, recipe, context, output_to_source));
                    break;

                default:
                    break;
            }
        }

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
        add_source_and_header_(str, recipe, false, std::list<model::Recipe*>(), output_to_source);
        str << ")" << std::endl;

        set_target_properties_(str, recipe, "PRIVATE", output_to_source);

        str << "# " << recipe->uri() << std::endl << std::endl;

        MSS_END();
    }

    Result CMake::add_static_library_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source)
    {
        MSS_BEGIN(Result);

        str << "# " << recipe->uri() << std::endl;

        str << "add_library(" << recipe_name_(recipe) << " STATIC" << std::endl;
        std::list<model::Recipe*> lst;
        MSS(context.menu().topological_order_recipes(recipe, lst));

        add_source_and_header_(str, recipe, false, lst, output_to_source);
        str << ")" << std::endl;

        set_target_properties_(str, recipe, "PRIVATE", output_to_source);

        str << "# " << recipe->uri() << std::endl << std::endl;
        MSS_END();
    }

    Result CMake::add_shared_library_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source)
    {
        MSS_BEGIN(Result);

        str << "# " << recipe->uri() << std::endl;

        str << "add_library(" << recipe_name_(recipe) << " SHARED" << std::endl;
        std::list<model::Recipe*> lst;
        MSS(context.menu().topological_order_recipes(recipe, lst));

        add_source_and_header_(str, recipe, true, lst, output_to_source);
        str << ")" << std::endl;

        set_target_properties_(str, recipe, "PRIVATE", output_to_source);

        str << "# " << recipe->uri() << std::endl << std::endl;
        MSS_END();
    }

    Result CMake::add_executable_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source)
    {
        MSS_BEGIN(Result);

        str << "# " << recipe->uri() << std::endl;
        set_link_paths_(str, recipe, output_to_source);

        str << "add_executable(" << recipe_name_(recipe) << " ";
        std::list<model::Recipe*> lst;
        MSS(context.menu().topological_order_recipes(recipe, lst));
        add_source_and_header_(str, recipe, false, lst, output_to_source);
        str << ")" << std::endl;
        set_target_properties_(str, recipe, "PRIVATE", output_to_source);

        MSS(set_link_libraries(str, recipe, lst, "PRIVATE", output_to_source));

        str << "# " << recipe->uri() << std::endl << std::endl;
        MSS_END();
    }

    Result CMake::add_interface_library_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source)
    {
        MSS_BEGIN(Result);
        str << "# " << recipe->uri() << std::endl;

        str << "add_library(" << recipe_name_(recipe) << " INTERFACE)" << std::endl;

        str << "target_sources(" << recipe_name_(recipe) << " INTERFACE" << std::endl;
        recipe->files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
                             {
                             if (ltp.type == Type::Header)
                             str << "  " << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.key()).string()) << std::endl;
                             return true;
                             });
        str << ")" << std::endl;

        set_target_properties_(str, recipe, "INTERFACE", output_to_source);


        str << "# " << recipe->uri() << std::endl << std::endl;
        MSS_END();
    }

    bool CMake::contains_sources_(const model::Recipe & recipe, CMakeType proposed_type) const
    {
        bool has_sources = false;
        recipe.files().each([&](const LanguageTypePair & ltp, const auto & ) {
                            has_sources = has_sources || (ltp.type == Type::Source);
                            return true;
                            });

        if (has_sources)
            return true;

        // check if any of the dependencies has sources
        if (proposed_type != CMakeType::Object)
        {
            for(const auto & dep : recipe.dependencies())
            {
                auto it = recipe_map_.find(dep);
                if (it != recipe_map_.end() && it->second == CMakeType::Object)
                    return true;
            }
        }
        return false;
    }
    std::string CMake::recipe_name_(model::Recipe * recipe) const
    {
        model::Uri uri = recipe->uri().as_relative();
        return uri.string('_');
    }

    Result CMake::add_source_and_header_(std::ostream & ofs, model::Recipe * recipe, bool add_exports, const std::list<model::Recipe*> & dependencies, const std::filesystem::path & output_to_source) const
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

                default:
                    break;
            }
            return true;
        };

        MSS(recipe->files().each(add_files));

        for(model::Recipe * dep : dependencies)
        {
            if (dep == recipe)
                break;

            auto it = recipe_map_.find(dep);
            if(it != recipe_map_.end() && it->second == CMakeType::Object)
                ofs << "  $<TARGET_OBJECTS:" << recipe_name_(dep) << ">" << std::endl; 
        }

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
        recipe->files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
                             {
                             if (ltp.type == Type::LibraryPath)
                             link_directories.push_back(gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.dir()).string()));

                             return true;
                             });

        write_elements_(oss, [](auto & os) { os << "link_directories("; }, link_directories, false);
    }

    Result CMake::set_link_libraries(std::ostream & ofs, model::Recipe * recipe, const std::list<model::Recipe*> & dependencies, const std::string & keyword, const std::filesystem::path & output_to_source) const
    {
        MSS_BEGIN(Result);

        const std::string & name = recipe_name_(recipe);

        // the link libraries
        {
            std::set<std::string> link_libraries;
            for(const ingredient::KeyValue & lib : recipe->key_values().range(LanguageTypePair(Language::Binary, Type::Library)))
            {
                // only the ones added by the user
                if (!is_internal_generated(lib.content()))
                    link_libraries.insert(gubg::stream([&](auto & os) { os << lib.key(); } ));
            }

            for (model::Recipe * dep : dependencies)
            {
                auto it = recipe_map_.find(dep);
                if (it != recipe_map_.end() && it->second == CMakeType::StaticLibrary)
                    link_libraries.insert(recipe_name_(dep));
            }


            write_elements_(ofs, [&](auto & os) { os << "target_link_libraries(" << name << " " << keyword; }, link_libraries, false);
        }

        MSS_END();
    }


    void CMake::set_target_properties_(std::ostream & ofs, model::Recipe * recipe, const std::string & keyword, const std::filesystem::path & output_to_source) const
    {
        const std::string & name = recipe_name_(recipe);

        // the include directories
        {
            std::set<std::string> include_dirs;
            recipe->files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
                                 {
                                 if (ltp.type == Type::IncludePath)
                                 {
                                 include_dirs.insert(gubg::stream([&](auto & os) { os << keyword << " " << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.key()).string()); } ));
                                 }
                                 return true;
                                 });
            write_elements_(ofs, [&](auto & os) { os << "target_include_directories(" << name; }, include_dirs, true);
        }


        // the compile definitions
        {
            std::set<std::string> definitions;
            recipe->key_values().each([&](const LanguageTypePair & ltp, const ingredient::KeyValue & key_value)
                                      {
                                      if (ltp.type == Type::Define)
                                      definitions.insert(key_value.to_string());
                                      return true;
                                      });

            write_elements_(ofs, [&](auto & os) { os << "target_compile_definitions(" << name << " " << keyword; }, definitions);
        }

        // the compile options
        {
            std::set<std::string> options;
            recipe->files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
                                 {
                                 if (ltp.type == Type::ForceInclude)
                                 options.insert(gubg::stream([&](auto & os)
                                                             {
                                                             os << "-include " << gubg::string::escape_cmake(gubg::filesystem::combine(output_to_source, file.rel()).string());
                                                             }));

                                 return true;
                                 });

            write_elements_(ofs, [&](auto & os) { os << "target_compile_options(" << name << " " << keyword; }, options);
        }

        // the dependencies
        {
            std::list<std::string> dependencies;
            for(model::Recipe * dep : recipe->dependencies())
            {
                if (recipe_map_.find(dep) != recipe_map_.end())
                    dependencies.push_back(recipe_name_(dep));
            }

            write_elements_(ofs, [&](auto & os) { os << "add_dependencies(" << name; }, dependencies, false);
        }
    }

} }


