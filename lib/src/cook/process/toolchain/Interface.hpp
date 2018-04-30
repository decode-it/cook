#ifndef HEADER_cook_process_toolchain_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Interface_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Types.hpp"
#include "cook/process/command/Compile.hpp"

namespace cook { namespace process { namespace toolchain { 


    class Interface
    {
    public:
        using Ptr = std::shared_ptr<Interface>;

        Interface()
        {
            kvm_[Part::Cli].emplace_back("", "");

            trans_.reset(new TranslatorMap);

            auto lambda = [&](toolchain::Part part){
                (*trans_)[part] = [](const std::string &, const std::string &){return "";};
            };
            toolchain::each_part(lambda);
        }
        virtual ~Interface() {}

        const KeyValuesMap &keyvalues_map() const { return kvm_; }
        const TranslatorMapPtr &translator_map() const { return trans_; }

        virtual bool set_brand(const std::string &) = 0;

        virtual bool create(command::Compile::Ptr &) const {return false;}

    protected:
        KeyValuesMap kvm_;
        TranslatorMapPtr trans_;
    };

} } } 

#endif
