#include "cook/app/Options.hpp"
#include "cook/Version.hpp"
#include "gubg/mss.hpp"
#include "gubg/OptionParser.hpp"
#include <algorithm>

namespace cook { namespace app {

namespace  {

Options::KeyValue parse_key_value_pair(const std::string & str)
{
    Options::KeyValue kv;

    const std::size_t pos = str.find('=');

    kv.first = str.substr(0, pos);

    if (pos < str.size()-1)
        kv.second = str.substr(pos+1);

    return kv;
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
        opt.add_mandatory(  't', "--toolchain-file       ", "The toolchain-file to use. If not found and any of [gcc,clang, msvc, default] a toolchain is generated in the working directory", [&](const std::string & str) { toolchains.push_back(str); });
        opt.add_mandatory(  'T', "--toolchain-option     ", "Passes the option to the toolchain.", [&](const std::string & str) { toolchain_options.push_back(parse_key_value_pair(str)); });
        opt.add_mandatory(  'g', "--generator            ", "A generator to use [naft|ninja|cmake|build]. If none are specified, then build is used.", [&](const std::string & str) { generators.push_back(parse_key_value_pair(str)); });
        opt.add_mandatory(  'C', "--chef                 ", "Chef to use [scal|cal|void]", [&](const std::string &str){ chef = str; });
        opt.add_switch(     'c', "--clean                ", "Clean the data for the specified generators before using them", [&](){ clean_ = true; });
        opt.add_mandatory(  'D', "--data                 ", "Passes the chaiscript variables to the process.", [&](const std::string & str) { variables.push_back(parse_key_value_pair(str)); });
        opt.add_switch(     'h', "--help                 ", "Prints this help.", [&](){ print_help = true; });
        opt.add_mandatory(  'v', "--verbosity            ", "Verbosity level, 0 is silent. By default this is 1. ", [&](const std::string & str) { verbosity = std::max(0, std::stoi(str)); });
    }

    // set the help message
    {
        std::ostringstream str;
        str << "cook version " << extended_version() << std::endl;
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

void Options::stream(log::Importance importance) const
{
    const auto imp = log::importance(importance);

    auto ss = log::scope("options", imp, [&](auto &n){
            n.attr("output_path", output_path);
            n.attr("temp_path", temp_path);
            n.attr("clean", (clean_ ? "true" : "false"));
            n.attr("print_help", (print_help ? "true" : "false"));
            n.attr("verbosity", verbosity);
            });

    auto log_single_range = [&](const std::string & name, const auto & range)
    {
        auto ss = log::scope(name, imp, [&](auto & n) {
            for(const auto & el : range)
                n.attr(el);
        });
    };

    auto log_pair_range = [&](const std::string & name, const auto & range)
    {
        auto ss = log::scope(name, imp, [&](auto & n) {
            for(const auto & el : range)
                if (!el.second)
                    n.attr(el.first);
                else
                    n.attr(el.first, *el.second);
        });
    };

    log_single_range("recipes", recipe_files);
    log_single_range("toolchains", toolchains);

    log_pair_range("toolchain-options", toolchain_options);
    log_pair_range("generators", generators);
    log_pair_range("variables", variables);
}

} }
