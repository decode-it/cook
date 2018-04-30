#ifndef HEADER_cook_process_toolchain_Types_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Types_hpp_ALREADY_INCLUDED

#include <memory>
#include <ostream>
#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <map>

namespace cook { namespace process { namespace toolchain { 

    enum class Part
    {
        Begin_,
        Cli = Begin_, Pre, Input, Output, DepFile, Option, Define, IncludePath, ForceInclude, Library, LibraryPath,
        End_
    };

    template <typename Ftor>
    void each_part(Ftor &&ftor)
    {
        for (unsigned int i = (unsigned int)Part::Begin_; i < (unsigned int)Part::End_; ++i)
        {
            auto part = (Part)i;
            ftor(part);
        }
    }

    using KeyValue = std::pair<std::string, std::string>;
    using KeyValues = std::vector<KeyValue>;
    using KeyValuesMap = std::map<Part, KeyValues>;

    using Translator = std::function<std::string (const std::string &, const std::string &)>;
    using TranslatorMap = std::map<Part, Translator>;
    using TranslatorMapPtr = std::shared_ptr<TranslatorMap>;

} } } 

#endif
