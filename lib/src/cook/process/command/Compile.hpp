#ifndef HEADER_cook_process_command_Compile_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Compile_hpp_ALREADY_INCLUDED

#include "cook/process/command/Interface.hpp"
#include "cook/process/toolchain/Types.hpp"
#include <memory>

namespace cook { namespace process { namespace command { 

    class Compile: public Interface
    {
    public:
        using Ptr = std::shared_ptr<Compile>;

        Compile(const toolchain::KeyValuesMap &kvm, const toolchain::TranslatorMapPtr &trans): kvm_(kvm), trans_(trans) {}
        ~Compile() {}

        std::string name() const override {return "Compile";}
        Type type() const override {return Type::Compile;}

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
                auto &depfiles = kvm_[toolchain::Part::DepFile];
                outputs.clear();
                depfiles.clear();
                for (const auto &fn: output_files)
                {
                    auto fn_str = fn.string();
                    outputs.emplace_back(fn_str, "");
                    fn_str += ".d";
                    depfiles.emplace_back(fn_str, "");
                }
            }
        }
        void stream_part(std::ostream & os, toolchain::Part part, const toolchain::Translator *trans_ptr = nullptr) const override
        {
            const auto &kvs = kvm_[part];
            const auto &trans = (!!trans_ptr ? *trans_ptr : (*trans_)[part]);
            for (const auto &kv: kvs)
            {
                const auto str = trans(kv.first, kv.second);
                if (!str.empty())
                    os << str << ' ';
            }
        }
        void stream_command(std::ostream & os) const override
        {
            toolchain::each_part([&](toolchain::Part part){ stream_part(os, part); });
        }
        Result process() override {return Result();}

        void add_define(const std::string & name, const std::string & value)
        {
            kvm_[toolchain::Part::Define].emplace_back(name, value);
        }
        void add_define(const std::string & name) { add_define(name, ""); }
        void add_include_path(const std::filesystem::path & path)
        {
            kvm_[toolchain::Part::IncludePath].emplace_back(path.string(), "");
        }
        void add_force_include(const std::filesystem::path & path)
        {
            kvm_[toolchain::Part::ForceInclude].emplace_back(path.string(), "");
        }

    private:
        mutable toolchain::KeyValuesMap kvm_;
        toolchain::TranslatorMapPtr trans_;
    };

} } } 

#endif
