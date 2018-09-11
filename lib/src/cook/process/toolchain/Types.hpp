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
        Cli = Begin_, Pre, Runtime, Deps, Export, Response, Output, Input, DepFile, Option, Define, IncludePath, ForceInclude, Library, LibraryPath, Framework, FrameworkPath, Resource,
        End_
    };

    inline std::ostream & operator<<(std::ostream & oss, Part p)
    {
        switch(p)
        {
#define L_CASE(NAME) case Part::NAME: return oss << #NAME
            L_CASE(Cli);
            L_CASE(Pre);
            L_CASE(Runtime);
            L_CASE(Deps);
            L_CASE(Export);
            L_CASE(Response);
            L_CASE(Output);
            L_CASE(Input);
            L_CASE(DepFile);
            L_CASE(Option);
            L_CASE(Define);
            L_CASE(IncludePath);
            L_CASE(ForceInclude);
            L_CASE(Library);
            L_CASE(LibraryPath);
            L_CASE(Framework);
            L_CASE(FrameworkPath);
            L_CASE(Resource);
#undef L_CASE
            default:
            return oss << "<unknown>";
        }
    }

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
