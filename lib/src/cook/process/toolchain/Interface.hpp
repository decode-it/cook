#ifndef HEADER_cook_process_toolchain_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Interface_hpp_ALREADY_INCLUDED

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
        Cli = Begin_, Pre, Inputs, Outputs, DepFile, Options, Defines, IncludePaths, ForceIncludes, Libraries, LibraryPaths,
        End_
    };

    using KeyValue = std::pair<std::string, std::string>;
    using KeyValues = std::vector<KeyValue>;
    using KeyValuesMap = std::map<Part, KeyValues>;

    using Translator = std::function<std::string (const std::string &, const std::string &)>;
    using TranslatorMap = std::map<Part, Translator>;
    using TranslatorMapPtr = std::shared_ptr<TranslatorMap>;

    class Interface
    {
    public:
        using Ptr = std::shared_ptr<Interface>;

        Interface()
        {
            kv_[Part::Cli].emplace_back("", "");

            trans_.reset(new TranslatorMap);
            Translator default_trans = [](const std::string &, const std::string &){return "";};
            for (unsigned int i = (unsigned int)Part::Begin_; i < (unsigned int)Part::End_; ++i)
            {
                auto part = (Part)i;
                (*trans_)[part] = default_trans;
            }
        }
        virtual ~Interface() {}

        const KeyValuesMap &keyvalues_map() const { return kv_; }
        const TranslatorMapPtr &translator_map() const { return trans_; }

        virtual bool set_brand(const std::string &) = 0;

    protected:
        KeyValuesMap kv_;
        TranslatorMapPtr trans_;
    };

} } } 

#endif
