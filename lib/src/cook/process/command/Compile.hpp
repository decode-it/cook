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
        Result process(const Filenames & input_files, const Filenames & output_files) override {return Result();}
        void stream_command(std::ostream & oss, const Filenames & input_files, const Filenames & output_files) const override
        {
            for (const auto &fn: input_files)
                kvm_[toolchain::Part::Input].emplace_back(fn.string(), "");
            for (const auto &fn: output_files)
            {
                auto fn_str = fn.string();
                kvm_[toolchain::Part::Output].emplace_back(fn_str, "");
                fn_str += ".d";
                kvm_[toolchain::Part::DepFile].emplace_back(fn_str, "");
            }

            auto lambda = [&](toolchain::Part part){
                const auto &kvs = kvm_[part];
                const auto &trans = (*trans_)[part];
                for (const auto &kv: kvs)
                {
                    const auto str = trans(kv.first, kv.second);
                    if (!str.empty())
                        oss << str << ' ';
                }
            };
            toolchain::each_part(lambda);
        }
        Type type() const override {return Type::Compile;}

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
