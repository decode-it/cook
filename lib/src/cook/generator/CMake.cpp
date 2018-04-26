#include "cook/generator/CMake.hpp"
#include "cook/Context.hpp"
#include "cook/util/File.hpp"
#include "gubg/stream.hpp"

namespace cook { namespace generator {

CMake::CMake()
{
    set_option("");
}

Result CMake::set_option(const std::string & option)
{
    MSS_BEGIN(Result);
    output_path_ = option;
    set_filename(gubg::filesystem::combine(output_path_, default_filename()).string());
    MSS_END();
}

bool CMake::can_process(const Context & context) const
{
    return context.menu().is_valid();
}

Result CMake::process(const Context & context)
{
    MSS_BEGIN(Result);

    std::ofstream ofs;
    MSS(open_output_stream(context, ofs));

    std::vector<std::pair<std::filesystem::path, std::ofstream>> streams;
    std::filesystem::path root_path = gubg::filesystem::normalize(std::filesystem::current_path());

    auto get_output_stream = [&](const std::filesystem::path & wd, std::ofstream *& str, bool & is_new)
    {
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

    const auto & recipe_list = context.menu().topological_order_recipes();

    ofs << "cmake_minimum_required (VERSION 3.1)" << std::endl;
    ofs << "project (recipes.chai)" << std::endl;
    ofs << "set(CMAKE_CXX_STANDARD 17)" << std::endl;

    for(const model::Recipe * recipe : recipe_list)
    {
        std::ofstream * str = nullptr;
        bool is_new = false;
        MSS(get_output_stream(recipe->working_directory(), str, is_new));
        MSS(!!str);

        if (is_new)
            ofs << "add_subdirectory(" << gubg::filesystem::normalize(recipe->working_directory()) << ")" << std::endl;


        std::filesystem::path output_to_source = gubg::filesystem::get_relative_to(output_path_ / recipe->working_directory(), recipe->working_directory());

        if (false) {}
        else if (recipe->build_target().type == TargetType::Executable)
            add_executable_(*str, *recipe, output_to_source);
        else if (contains_sources_(*recipe))
            add_library_(*str, *recipe, output_to_source);
        else
            add_interface_(*str, *recipe, output_to_source);

    }

    MSS_END();
}

std::string CMake::default_filename() const
{
    return "CMakeLists.txt";
}

void CMake::add_interface_(std::ostream &ofs, const model::Recipe & recipe, const std::filesystem::path & output_to_source) const
{
    ofs << "# " << recipe.uri() << std::endl;

    ofs << "add_library(" << recipe_name_(recipe) << " INTERFACE" << std::endl;
    add_source_and_header_(ofs, recipe, false, output_to_source);
    ofs << ")" << std::endl;

    ofs << "target_sources(" << recipe_name_(recipe) << " INTERFACE" << std::endl;
    recipe.files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
    {
        if (ltp.type == Type::Header)
            ofs << "  " << gubg::filesystem::combine(output_to_source, file.key()) << std::endl;
        return true;
    });
    ofs << ")" << std::endl;

    set_target_properties_(ofs, recipe, "INTERFACE", output_to_source);


    ofs << "# " << recipe.uri() << std::endl << std::endl;
}

void CMake::add_library_(std::ostream & ofs, const model::Recipe & recipe, const std::filesystem::path &output_to_source) const
{
    ofs << "# " << recipe.uri() << std::endl;
    if (contains_sources_(recipe))
    {
        set_link_paths_(ofs, recipe, output_to_source);

        ofs << "add_library(" << recipe_name_(recipe) << " " << deduce_library_type_(recipe) << std::endl;
        add_source_and_header_(ofs, recipe, true, output_to_source);
        ofs << ")" << std::endl;
        set_target_properties_(ofs, recipe, "PRIVATE", output_to_source);
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

void CMake::add_executable_(std::ostream & ofs, const model::Recipe & recipe, const std::filesystem::path & output_to_source) const
{
    ofs << "# " << recipe.uri() << std::endl;
    if (contains_sources_(recipe))
    {
        set_link_paths_(ofs, recipe, output_to_source);

        ofs << "add_executable(" << recipe_name_(recipe) << " ";
        add_source_and_header_(ofs, recipe, true, output_to_source);
        ofs << ")" << std::endl;
        set_target_properties_(ofs, recipe, "PRIVATE", output_to_source);
    }

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

void CMake::add_source_and_header_(std::ostream & ofs, const model::Recipe & recipe, bool add_headers, const std::filesystem::path & output_to_source) const
{
    recipe.files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
    {
        switch(ltp.type)
        {
            case Type::Source:
                ofs << "  " << gubg::filesystem::combine(output_to_source, file.key()) << std::endl;
                break;

            case Type::Header:
                if (add_headers)
                    ofs << "  " << gubg::filesystem::combine(output_to_source, file.key()) << std::endl;

                break;

            default:
                break;
        }
        return true;
    });
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

void CMake::set_link_paths_(std::ostream & oss, const model::Recipe & recipe, const std::filesystem::path & output_to_source) const
{
    std::list<std::string> link_directories;
    recipe.files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
    {
        if (ltp.type == Type::LibraryPath && file.owner() == nullptr)
            link_directories.push_back(gubg::filesystem::combine(output_to_source, file.dir()).string());

        return true;
    });

    write_elements_(oss, [](auto & os) { os << "link_directories("; }, link_directories, false);
}

void CMake::set_target_properties_(std::ostream & ofs, const model::Recipe & recipe, const std::string & keyword, const std::filesystem::path & output_to_source) const
{
    const std::string & name = recipe_name_(recipe);

    // the include directories
    {
        std::set<std::string> include_dirs;
        recipe.files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
        {
            if (ltp.type == Type::IncludePath)
            {
                include_dirs.insert(gubg::stream([&](auto & os) { os << keyword << " " << gubg::filesystem::combine(output_to_source, file.key()); } ));
            }
            return true;
        });
        write_elements_(ofs, [&](auto & os) { os << "target_include_directories(" << name; }, include_dirs, true);
    }


    // the link libraries
    {
        std::set<std::string> link_libraries;
        for(const ingredient::KeyValue & lib : recipe.key_values().range(LanguageTypePair(Language::Binary, Type::Library)))
        {
            if (false) {}
            else if (lib.owner() == nullptr)
                link_libraries.insert(gubg::stream([&](auto & os) { os << lib.key(); } ));

            else if (lib.owner() != &recipe)
                link_libraries.insert(gubg::stream([&](auto & os) { os << recipe_name_(*lib.owner()); } ));
        }

        write_elements_(ofs, [&](auto & os) { os << "target_link_libraries(" << name << " " << keyword; }, link_libraries, false);
    }



    // the compile definitions
    {
        std::set<std::string> definitions;
        recipe.key_values().each([&](const LanguageTypePair & ltp, const ingredient::KeyValue & key_value)
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
        recipe.files().each([&](const LanguageTypePair & ltp, const ingredient::File & file)
        {
            if (ltp.type == Type::ForceInclude)
                options.insert(gubg::stream([&](auto & os)
                {
                    os << "-include " << gubg::filesystem::combine(output_to_source, file.rel()).string();
                }));

            return true;
        });

        write_elements_(ofs, [&](auto & os) { os << "target_compile_options(" << name << " " << keyword; }, options);
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


