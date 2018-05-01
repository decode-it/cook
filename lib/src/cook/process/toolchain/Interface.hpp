#ifndef HEADER_cook_process_toolchain_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Interface_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Types.hpp"
#include "cook/process/command/Compile.hpp"
#include "cook/process/command/Archive.hpp"
#include "cook/process/command/Link.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace process { namespace toolchain { 

    using ConfigureFunction = std::function<bool (const std::string &, const std::string &, KeyValuesMap &, TranslatorMap &)>;

    //Chain of responsibility for translating and applying configuration settings
    class Configure_cor
    {
    public:
        template <typename Ftor>
        void add(Ftor &&ftor) { functions_.emplace_back(ftor); }

        bool operator()(const std::string &k, const std::string &v, KeyValuesMap &kvm, TranslatorMap &trans) const
        {
            for (const auto &ftor: functions_)
            {
                if (ftor(k, v, kvm, trans))
                    return true;
            }
            return false;
        }

    private:
        std::list<ConfigureFunction> functions_;
    };

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

        virtual bool configure(const std::string &key, const std::string &value)
        {
            MSS_BEGIN(bool);
            MSS(!!trans_);
            MSS(configure_(key, value, kvm_, *trans_));
            MSS_END();
        }

        virtual bool create(command::Compile::Ptr &) const {return false;}
        virtual bool create(command::Archive::Ptr &) const {return false;}
        virtual bool create(command::Link::Ptr &) const {return false;}

    protected:
        KeyValuesMap kvm_;
        TranslatorMapPtr trans_;
        Configure_cor configure_;
    };

} } } 

#endif
