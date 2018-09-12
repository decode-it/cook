#include "cook/process/command/CommonImpl.hpp"

namespace cook { namespace process { namespace command { 

    CommonImpl::CommonImpl(const toolchain::KeyValuesMap &kvm, const toolchain::TranslatorMapPtr &trans, Language language): kvm_(kvm), trans_(trans), language_(language) {}

    void CommonImpl::set_inputs_outputs(const Filenames & input_files, const Filenames & output_files) 
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
    std::string CommonImpl::get_kv_part(toolchain::Part part, const std::string & key, const std::string & value, toolchain::Translator * trans_ptr) const
    {
        const auto &trans = (!!trans_ptr ? *trans_ptr : (*trans_)[part]);
        return trans(key, value);
    }

    bool CommonImpl::stream_part(std::ostream & os, toolchain::Part part, const toolchain::Translator *trans_ptr, const PartEscapeFunctor & functor) const 
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
                if (functor)
                    os << functor(part, str);
                else
                    os << str;
            }
        }
        return true;
    }
    void CommonImpl::stream_command(std::ostream & os, const PartEscapeFunctor & functor) const 
    {

        auto lambda = [&](toolchain::Part part) 
        { 
            if (stream_part(os, part, nullptr, functor)) 
                os << ' '; 
        };
        toolchain::each_part(lambda);
    }

} } } 

