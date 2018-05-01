#ifndef HEADER_cook_process_command_Link_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Link_hpp_ALREADY_INCLUDED

#include "cook/process/command/Interface.hpp"
#include "cook/process/toolchain/Types.hpp"
#include <memory>

namespace cook { namespace process { namespace command { 

    class Link: public Interface
    {
    public:
        using Ptr = std::shared_ptr<Link>;

        Link(const toolchain::KeyValuesMap &kvm, const toolchain::TranslatorMapPtr &trans): kvm_(kvm), trans_(trans) {}
        ~Link() {}

        std::string name() const override {return "Link";}
        Type type() const override {return Type::Link;}

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

        void add_library(const std::string & name)
        {
            kvm_[toolchain::Part::Library].emplace_back(name, "");
        }
        void add_library_path(const std::filesystem::path & path)
        {
            kvm_[toolchain::Part::LibraryPath].emplace_back(path.string(), "");
        }

    private:
        mutable toolchain::KeyValuesMap kvm_;
        toolchain::TranslatorMapPtr trans_;
    };

} } } 

#endif
