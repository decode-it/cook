#include "cook/process/command/CommonImpl.hpp"
#include "cook/util/File.hpp"
#include <cassert>

namespace cook { namespace process { namespace command {

        CommonImpl::CommonImpl(toolchain::Element::Ptr ptr)
            : ptr_(ptr)
            , kvm_(ptr_->key_values_map())
            , trans_(ptr->translator_map())
            , language_(ptr_->language())
            , recipe_path_(util::get_from_to_path(".", ptr->recipe()))
        {}

    void CommonImpl::set_inputs_outputs(const Filenames& input_files, const Filenames& output_files)
    {
        {
            auto& inputs = kvm_[toolchain::Part::Input];
            inputs.clear();
            for (const auto& fn : input_files)
                inputs.emplace_back(escape_spaces(fn.string()), "");
        }
        {
            auto& outputs = kvm_[toolchain::Part::Output];
            outputs.clear();
            for (const auto& fn : output_files)
                outputs.emplace_back(escape_spaces(fn.string()), "");
        }
    }
    std::string CommonImpl::get_kv_part(toolchain::Part part, const std::string & key, const std::string & value, toolchain::Translator * trans_ptr) const
    {
        auto it = trans_.find(part);
        assert(it != trans_.end());
        const auto &trans = (!!trans_ptr ? *trans_ptr : it->second);
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

        auto it = trans_.find(part);
        assert(it != trans_.end());
        const auto &trans = (!!trans_ptr ? *trans_ptr : it->second);
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
    
    std::string CommonImpl::escape_spaces(const std::string & str)
    {
        if (str.find_first_of(' ') == std::string::npos)
            return str;

        std::string res(str.size() + 2, '\"');
        std::copy(str.begin(), str.end(), res.begin()+1);
        return res;
    }
        
    bool CommonImpl::process_ingredient(const LanguageTypePair& ltp, const ingredient::File& file)
    {
        ingredient::File f(file, recipe_path_);
        return ptr_->process_ingredient(ltp, f); 
    }

    bool CommonImpl::process_ingredient(const LanguageTypePair& ltp, const ingredient::KeyValue& key_value)
    {
        return ptr_->process_ingredient(ltp, key_value); 
    }

} } } 

