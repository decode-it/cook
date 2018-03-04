#include "cook/generator/CMake.hpp"
#include "cook/Context.hpp"
#include "cook/util/File.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace generator {

Result CMake::set_option(const std::string & option)
{
    MSS_BEGIN(Result);
    set_filename(option);
    MSS_END();
}

bool CMake::can_process(const Context & context) const
{
    return context.menu().is_valid();
}

Result CMake::process(std::ostream & ofs, const Context & context)
{
    MSS_BEGIN(Result);

    const auto & recipe_list = context.menu().topological_order_recipes();

    ofs << "cmake_minimum_required (VERSION 3.1)" << std::endl;
    ofs << "project (recipes.chai)" << std::endl;
    ofs << "set(CMAKE_CXX_STANDARD 17)" << std::endl;

    for(const model::Recipe * recipe : recipe_list)
    {
        if (false) {}
        else if (recipe->type() == Type::Executable)
            add_executable_(ofs, *recipe);
        else if (contains_sources_(*recipe))
            add_library_(ofs, *recipe);
        else
            add_interface_(ofs, *recipe);

    }

    MSS_END();
}

std::string CMake::default_filename() const
{
    return "CMakeLists.txt";
}

void CMake::add_interface_(std::ostream &ofs, const model::Recipe & recipe) const
{
    ofs << "# " << recipe.uri() << std::endl;

    ofs << "add_library(" << recipe_name_(recipe) << " INTERFACE" << std::endl;
    add_source_and_header_(ofs, recipe, false);
    ofs << ")" << std::endl;

    set_target_properties_(ofs, recipe, "INTERFACE");


    ofs << "# " << recipe.uri() << std::endl << std::endl;
}

void CMake::add_library_(std::ostream & ofs, const model::Recipe & recipe) const
{
    ofs << "# " << recipe.uri() << std::endl;
    if (contains_sources_(recipe))
    {
        set_link_paths_(ofs, recipe);

        ofs << "add_library(" << recipe_name_(recipe) << " " << deduce_library_type_(recipe) << std::endl;
        add_source_and_header_(ofs, recipe, false);
        ofs << ")" << std::endl;
        set_target_properties_(ofs, recipe, "PRIVATE");
    }

    ofs << "# " << recipe.uri() << std::endl << std::endl;
}

bool CMake::contains_sources_(const model::Recipe & recipe) const
{
    bool has_sources = false;
    recipe.files().each([&](const LanguageTypePair & ltp, const auto & ) {
        has_sources = has_sources || (ltp.type == Type::Source);
        return true;
    });
    return has_sources;
}

void CMake::add_executable_(std::ostream & ofs, const model::Recipe & recipe) const
{
    ofs << "# " << recipe.uri() << std::endl;
    set_link_paths_(ofs, recipe);

    ofs << "add_executable(" << recipe_name_(recipe) << " ";
    add_source_and_header_(ofs, recipe, true);
    ofs << ")" << std::endl;
    set_target_properties_(ofs, recipe, "PRIVATE");

    ofs << "# " << recipe.uri() << std::endl << std::endl;
}

std::string CMake::recipe_name_(const model::Recipe & recipe) const
{
    model::Uri uri = recipe.uri().as_relative();
    return uri.string('_');
}

std::string CMake::deduce_library_type_(const model::Recipe & recipe) const
{
//    if (recipe.type() == Type::Library)
//        return "SHARED";
//    else
        return "STATIC";
}

void CMake::add_source_and_header_(std::ostream & ofs, const model::Recipe & recipe, bool add_headers) const
{
    recipe.files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
    {
        switch(ltp.type)
        {
            case Type::Source:
                ofs << "  " << file.key() << std::endl;
                break;

            case Type::Header:
                if (add_headers)
                    ofs << "  " << file.key() << std::endl;

                break;

            default:
                break;
        }
        return true;
    });
}



namespace  {

template <typename OpenFunction>
void write_elements_(std::ostream & ofs, OpenFunction && open_function, const std::list<std::string> & elements, bool new_line = true)
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

void CMake::set_link_paths_(std::ostream & oss, const model::Recipe & recipe) const
{
    std::list<std::string> link_directories;
    recipe.files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
    {
        if (ltp.type == Type::LibraryPath)
            link_directories.push_back(file.dir().string());
        return true;
    });

    write_elements_(oss, [](auto & os) { os << "link_directories("; }, link_directories, false);
}

void CMake::set_target_properties_(std::ostream & ofs, const model::Recipe & recipe, const std::string & keyword) const
{
    const std::string & name = recipe_name_(recipe);

    // the include directories
    {
        std::list<std::string> include_dirs;
        recipe.files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
        {
            if (ltp.type == Type::IncludePath)
            {
                include_dirs.push_back(gubg::stream([&](auto & os) { os << keyword << " " << file.key(); } ));
            }
            return true;
        });
        write_elements_(ofs, [&](auto & os) { os << "target_include_directories(" << name; }, include_dirs, true);
    }


    // the link libraries
    {
        std::list<std::string> link_libraries;
        for(const ingredient::File & file : recipe.files().range(LanguageTypePair(Language::Binary, Type::Library)))
        {
            if (false) {}
            else if (file.owner() == nullptr)
                link_libraries.push_back(gubg::stream([&](auto & os) { os << file.key(); } ));
            else if (file.owner() != &recipe)
                link_libraries.push_back(gubg::stream([&](auto & os) { os << recipe_name_(*file.owner()); } ));
        }

        write_elements_(ofs, [&](auto & os) { os << "target_link_libraries(" << name << " " << keyword; }, link_libraries, false);
    }



    // the compile definitions libraries
    {
        std::list<std::string> definitions;
        recipe.key_values().each([&](const LanguageTypePair & ltp, const ingredient::KeyValue & key_value)
        {
            if (ltp.type == Type::Define)
                definitions.push_back(key_value.to_string());
            return true;
        });

        write_elements_(ofs, [&](auto & os) { os << "target_compile_definitions(" << name << " " << keyword; }, definitions);
    }

    // the dependencies
    {
        std::list<std::string> dependencies;
        for(const model::Recipe * dep : recipe.dependencies())
            dependencies.push_back(recipe_name_(*dep));

        write_elements_(ofs, [&](auto & os) { os << "add_dependencies(" << name; }, dependencies, false);
    }
}

} }


