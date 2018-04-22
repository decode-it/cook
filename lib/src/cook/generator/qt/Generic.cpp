#include "cook/generator/qt/Generic.hpp"
#include "cook/util/File.hpp"

namespace cook { namespace generator { namespace qt {

Result Generic::set_option(const std::string & option)
{
    MSS_BEGIN(Result);
    set_filename(option);
    MSS_END();
}

bool Generic::can_process(const Context & context) const
{
    return context.menu().is_valid();
}

Result Generic::open_output_stream(const std::string & project_name, const std::string & postfix, std::ofstream & ofs) const
{
    MSS_BEGIN(Result);
    std::filesystem::path fn = gubg::filesystem::combine(output_dir_, project_name + "." + postfix);
    MSS(util::open_file(fn, ofs));

    MSS_END();
}

Result Generic::process(const Context & context)
{
    MSS_BEGIN(Result);

    std::ofstream ofiles;
    MSS(open_output_stream(context.project_name(), "files", ofiles));

    std::ofstream oincludes;
    MSS(open_output_stream(context.project_name(), "includes", oincludes));

    std::ofstream oconfig;
    MSS(open_output_stream(context.project_name(), "config", oconfig));

    for(const model::Recipe * recipe : context.menu().topological_order_recipes())
    {
        const std::filesystem::path output_to_recipe = gubg::filesystem::get_relative_to(output_dir_, recipe->working_directory());

        MSS(write_files_(ofiles, *recipe, output_to_recipe));
        MSS(write_includes_(oincludes, *recipe, output_to_recipe));
        MSS(write_config_(oconfig, *recipe));

    }

    MSS_END();
}

Result Generic::write_files_(std::ofstream & ofs, const model::Recipe & recipe, const std::filesystem::path & to_recipe) const
{
    return recipe.files().each([&](const LanguageTypePair & ltp, const ingredient::File & file) {

        switch(ltp.type)
        {
        case Type::Header:
        case Type::Source:
            ofs << gubg::filesystem::combine(to_recipe, file.key()).string() << std::endl;
            break;

        default:
            break;
        }

        return true;
    });
}

Result Generic::write_config_(std::ofstream & ofs, const model::Recipe & recipe) const
{
    return recipe.key_values().each([&](const LanguageTypePair & ltp, const ingredient::KeyValue & kv) {

        switch(ltp.type)
        {
            case Type::Define:
                ofs << "#define " << kv.key();
                if (kv.has_value())
                    ofs << " " << kv.value();
                ofs << std::endl;
                break;

        default:
            break;
        }

        return true;
    });
}

Result Generic::write_includes_(std::ofstream & ofs, const model::Recipe & recipe, const std::filesystem::path & to_recipe) const
{
    return recipe.files().each([&](const LanguageTypePair & ltp, const ingredient::File & file) {

        switch(ltp.type)
        {
        case Type::IncludePath:
            ofs << gubg::filesystem::combine(to_recipe, file.dir()).string() << std::endl;
            break;

        default:
            break;
        }

        return true;
    });
}

} } }


