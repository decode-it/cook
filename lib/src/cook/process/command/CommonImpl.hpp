#ifndef HEADER_cook_process_command_CommonImpl_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_CommonImpl_hpp_ALREADY_INCLUDED

#include "cook/process/command/Interface.hpp"
#include "cook/process/toolchain/Types.hpp"
#include "gubg/OnlyOnce.hpp"

namespace cook { namespace process { namespace command { 

    class CommonImpl: public Interface
    {
    public:
        CommonImpl(const toolchain::KeyValuesMap &kvm, const toolchain::TranslatorMapPtr &trans): kvm_(kvm), trans_(trans) {}

        void set_inputs_outputs(const Filenames & input_files, const Filenames & output_files) override
        {
            {
                auto &inputs = kvm_[toolchain::Part::Input];
                inputs.clear();
                for (const auto &fn: input_files)
                    inputs.emplace_back(fn.string(), "");
            }
            {
                auto &outputs = kvm_[toolchain::Part::Output];
                outputs.clear();
                for (const auto &fn: output_files)
                    outputs.emplace_back(fn.string(), "");
            }
        }
        bool stream_part(std::ostream & os, toolchain::Part part, const toolchain::Translator *trans_ptr = nullptr) const override
        {
            auto kvs_it = kvm_.find(part);
            if (kvs_it == kvm_.end())
                return false;
            const auto &kvs = kvs_it->second;
            if (kvs.empty())
                return false;
            const auto &trans = (!!trans_ptr ? *trans_ptr : (*trans_)[part]);
            gubg::OnlyOnce skip_space;
            for (const auto &kv: kvs)
            {
                const auto str = trans(kv.first, kv.second);
                if (!str.empty())
                {
                    if (!skip_space())
                        os << ' ';
                    os << str;
                }
            }
            return true;
        }
        void stream_command(std::ostream & os) const override
        {
            toolchain::each_part([&](toolchain::Part part){ if (stream_part(os, part)) os << ' '; });
        }

    protected:
        toolchain::KeyValuesMap kvm_;
        toolchain::TranslatorMapPtr trans_;
    };

} } } 

#endif
