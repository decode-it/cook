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
        Result process(const Filenames & input_files, const Filenames & output_files) override {return Result();}
        void stream_command(std::ostream & oss, const Filenames & input_files, const Filenames & output_files) const override
        {
            for (const auto &fn: input_files)
                kvm_[toolchain::Part::Input].emplace_back(fn.string(), "");
            for (const auto &fn: output_files)
                kvm_[toolchain::Part::Output].emplace_back(fn.string(), "");

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
        Type type() const override {return Type::Link;}

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
