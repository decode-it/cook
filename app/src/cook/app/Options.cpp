#include "cook/app/Options.hpp"
#include "cook/app/Version.hpp"
#include "gubg/mss.hpp"
#include "gubg/OptionParser.hpp"

namespace cook { namespace app {

namespace  {

std::pair<std::string, std::string> parse_key_value_pair(const std::string & str)
{
    const std::size_t pos = str.find('=');

    if (pos < str.size()-1)
        return std::make_pair(str.substr(0, pos), str.substr(pos+1));
    else
        return std::make_pair(str.substr(0, pos), std::string());
}

}

bool Options::parse(int argc, const char ** argv)
{
    MSS_BEGIN(bool);
    MSS(!parsed_);

    // create the options
    gubg::OptionParser opt("Cooking fresh binaries, C++-style");
    {
        opt.add_mandatory(  'f', "--input-file           ", "recipes to process", [&](const std::string & str) { recipe_files.push_back(str); });
        opt.add_mandatory(  'o', "--output-dir           ", "Output directory, default is \"./\"", [&](const std::string & str) { output_path = str; });
        opt.add_mandatory(  'O', "--temp-dir             ", "Temporary build cache directory. Default is .cook", [&](const std::string & str) {temp_path = str; });
        opt.add_mandatory(  't', "--toolchain            ", "The toolchain to use.", [&](const std::string & str) { toolchain = str; });
        opt.add_mandatory(  'g', "--generator            ", "A generator to use. If none are specified, then build is used.", [&](const std::string & str) { generators.push_back(parse_key_value_pair(str)); });
        opt.add_mandatory(  'd', "--dependency-visualizer", "A dependency visualizer to use.", [&](const std::string & str) { visualizers.push_back(parse_key_value_pair(str)); });
        opt.add_switch(     'c', "--clean                ", "Clean the data for the specified generators before using them", [&](){ clean = true; });
        opt.add_mandatory(  'D', "--data                 ", "Passes the chaiscript variables to the process.", [&](const std::string & str) { variables.push_back(parse_key_value_pair(str)); });
        opt.add_switch(     'h', "--help                 ", "Prints this help.", [&](){ print_help = true; });
        opt.add_mandatory(  'v', "--verbosity            ", "Verbosity level, 0 is silent. By default this is 1. ", [&](const std::string & str) { verbosity = std::max(0, std::stoi(str)); });
    }

    // set the help message
    {
        std::ostringstream str;
        str << "cook version " << version() << std::endl;
        str << opt.help();
        help_message = str.str();
    }

    // parse the arguments
    auto args = gubg::OptionParser::create_args(argc, argv);
    MSS(opt.parse(args));

    // remaining elements are recipe names
    recipes.assign(args.begin(), args.end());

    if (recipe_files.empty())
        recipe_files.push_back("recipes.chai");

    parsed_ = true;
    MSS_END();
}

bool Options::valid() const
{
    return parsed_;
}

void Options::stream(std::ostream &os, Indent &indent) const
{
    os << indent << "[options]{" << std::endl;

    {
        auto scope = indent.scope();

        os << indent << "[recipe_files]";
        for (const auto &fn: recipe_files)
            os << "(" << fn << ")";
        os << std::endl;

        os << indent << "[output_path](" << output_path << ")" << std::endl;
        os << indent << "[temp_path](" << temp_path << ")" << std::endl;
        os << indent << "[toolchain](" << toolchain << ")" << std::endl;

        os << indent << "[generators]";
        for (const auto &p: generators)
            os << "(" << p.first << ":" << p.second << ")";
        os << std::endl;

        os << indent << "[visualizers]";
        for (const auto &p: visualizers)
            os << "(" << p.first << ":" << p.second << ")";
        os << std::endl;

        os << indent << "[variables]";
        for (const auto &p: variables)
            os << "(" << p.first << ":" << p.second << ")";
        os << std::endl;

        os << indent << "[recipes]";
        for (const auto &r: recipes)
            os << "(" << r << ")";
        os << std::endl;

        os << indent << "[clean](" << (clean ? "true" : "false") << ")" << std::endl;
        os << indent << "[print_help](" << (print_help ? "true" : "false") << ")" << std::endl;
        os << indent << "[verbosity](" << verbosity << ")" << std::endl;
    }

    os << "}" << std::endl;
}

} }
