#ifndef HEADER_cook_model_Globber_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Globber_hpp_ALREADY_INCLUDED

#include "cook/model/property/File.hpp"
#include "cook/model/property/PropertiesKey.hpp"
#include "cook/Propagation.hpp"
#include "cook/Overwrite.hpp"
#include "cook/Language.hpp"
#include "cook/Type.hpp"
#include "gubg/file/System.hpp"
#include <string>
#include <optional>
#include <functional>
#include <algorithm>


namespace cook { namespace model {

struct Globber
{
    std::string dir;
    std::string pattern;

    Language language = Language::Undefined;
    Type type = Type::Undefined;

    std::optional<Propagation> propagation;
    std::optional<Overwrite> overwrite;
    std::function<bool (property::PropertiesKey &, property::File &)> filter_and_adaptor;

    template <typename Resolver, typename Adder>
    bool process(const std::filesystem::path & wd, Resolver && resolve, Adder && add) const
    {
        MSS_BEGIN(bool);

        // get the directory
        std::filesystem::path wd_dir = dir;
        if (wd_dir.is_relative())
            wd_dir = wd / wd_dir;

        // find all dirs matching the pattern
        auto file_callback = [&](const std::filesystem::path & fn)
        {
            // extract the dir and the fn part
            std::filesystem::path f_dir, f_rel;
            extract_dir(gubg::make_range(fn), gubg::make_range(wd_dir), f_dir, f_rel);

            // resolve this file type
            property::File file(f_dir, f_rel);
            property::PropertiesKey key(language, type);

            resolve(key, file);

            // overwrite the specifications
            if (propagation)    file.set_propagation(*propagation);
            if (overwrite)      file.set_overwrite(*overwrite);

            if (!filter_and_adaptor || filter_and_adaptor(key, file))
                add(key, file);

            return true;
        };

        MSS(gubg::file::each_glob(pattern, file_callback, wd_dir));

        MSS_END();
    }

private:
    template <typename It>
    void extract_dir(
            gubg::Range<It> fn_rng,
            gubg::Range<It> dir_rng,
            std::filesystem::path & actual_dir,
            std::filesystem::path & actual_rel) const
    {
        // process both filesystems until a difference is found
        while(!dir_rng.empty() && !fn_rng.empty() && dir_rng.front() == fn_rng.front())
        {
            // add to this directory
            actual_dir /= (*dir_rng.begin());

            dir_rng.pop_front();
            fn_rng.pop_front();
        }

        // match the end
        for(const auto & part : fn_rng)
            actual_rel /= (part);
    }
};

} }

#endif
