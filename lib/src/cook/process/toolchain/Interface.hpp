#ifndef HEADER_cook_process_toolchain_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Interface_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Types.hpp"
#include "cook/process/command/Compile.hpp"
#include "cook/process/command/Archive.hpp"
#include "cook/process/command/Link.hpp"
#include "cook/process/toolchain/Configuration.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace process { namespace toolchain { 

    class Interface : public Configuration
    {
    public:
        using Ptr = std::shared_ptr<Interface>;

        Interface()
            : trans_(std::make_shared<TranslatorMap>())
        {
            kvm_[Part::Cli].emplace_back("", "");

            auto lambda = [&](toolchain::Part part)
            {
                (*trans_)[part] = [](const std::string &, const std::string &){return "";};
            };
            toolchain::each_part(lambda);
        }
        virtual ~Interface() {}

        const TranslatorMapPtr &translator_map_ptr() const { return trans_; }

        virtual bool create(command::Compile::Ptr &) const  { return false; }
        virtual bool create(command::Archive::Ptr &) const  { return false; }
        virtual bool create(command::Link::Ptr &) const     { return false; }

        KeyValuesMap & key_values_map() { return kvm_; }
        TranslatorMap & translator_map() { return *trans_; }
        const KeyValuesMap & key_values_map() const { return kvm_; }
        const TranslatorMap & translator_map() const { return *trans_; }

    private:
       Interface(const Interface &) = delete;
        Interface(Interface &&) = delete;
        Interface & operator=(const Interface &) = delete;
        Interface & operator=(Interface &&) = delete;
        
        KeyValuesMap kvm_;
        TranslatorMapPtr trans_;
    };

} } } 

#endif
