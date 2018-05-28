#ifndef HEADER_cook_generator_CMake_hpp_ALREADY_INCLUDED
#define HEADER_cook_generator_CMake_hpp_ALREADY_INCLUDED

#include "cook/generator/Interface.hpp"
#include "cook/model/Recipe.hpp"
#include "cook/model/Dirs.hpp"

namespace cook { namespace generator {

class CMake : public Interface
{
public:
    enum class CMakeType
    {
        Object,
        StaticLibrary,
        SharedLibrary,
        Interface,
        Executable,
        Skip
    };
    
    CMake();

    std::string name() const override { return "CMake"; }

    Result set_option(const std::string & option) override;

    bool can_process(const Context & context) const override;
    Result process(const Context & context) override;

private:
    using RecipeTypeMap = std::map<const model::Recipe *, CMakeType>;
    Result construct_recipe_map_(const Context & context); 
    Result check_types_(const Context & context);
    Result add_object_library_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source);
    Result add_static_library_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source);
    Result add_shared_library_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source);
    Result add_executable_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source);
    Result add_interface_library_(std::ofstream & str, model::Recipe * recipe, const Context & context, const std::filesystem::path & output_to_source);
    //Result can_build_recipe_(const model::Recipe & recipe) const;
    
    std::string default_filename() const override;
/*
    Result add_library_(std::ostream &ofs, const model::Recipe & recipe, const std::filesystem::path & output_to_source) const;
    Result add_executable_(std::ostream &ofs, const model::Recipe & recipe, const std::filesystem::path &output_to_source) const;
    Result add_interface_(std::ostream & ofs, const model::Recipe & recipe, const std::filesystem::path & output_to_source) const;
*/
    std::string recipe_name_(model::Recipe * recipe) const;
//    std::string deduce_library_type_(const model::Recipe & recipe) const;

    Result add_source_and_header_(std::ostream & ofs, model::Recipe * recipe, bool add_exports,  const std::list<model::Recipe*> & dependencies, const std::filesystem::path &output_to_source) const;
    bool set_target_properties_(std::ostream & ofs, model::Recipe * recipe, const std::string &keyword, const std::filesystem::path & output_to_source) const;
    void set_link_paths_(std::ostream & oss, model::Recipe * recipe, const std::filesystem::path & output_to_source) const;
    Result set_link_libraries(std::ostream & ofs, model::Recipe * recipe, const std::list<model::Recipe*> & dependencies, const std::string & keyword, const std::filesystem::path & output_to_source) const;

    bool contains_sources_(const model::Recipe & recipe, CMakeType propsed_type) const;

    std::filesystem::path output_path_;
    RecipeTypeMap recipe_map_;
    const Context * context_;
};

} }

#endif
