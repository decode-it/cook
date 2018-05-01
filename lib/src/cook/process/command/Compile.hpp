#ifndef HEADER_cook_process_command_Compile_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Compile_hpp_ALREADY_INCLUDED

#include "cook/process/command/CommonImpl.hpp"
#include "cook/process/toolchain/Types.hpp"
#include "gubg/OnlyOnce.hpp"
#include <memory>

namespace cook { namespace process { namespace command { 

    class Compile: public CommonImpl
    {
    public:
        using Ptr = std::shared_ptr<Compile>;

        Compile(const toolchain::KeyValuesMap &kvm, const toolchain::TranslatorMapPtr &trans): CommonImpl(kvm, trans) {}

        std::string name() const override {return "Compile";}
        Type type() const override {return Type::Compile;}

        void set_inputs_outputs(const Filenames & input_files, const Filenames & output_files) override
        {
            CommonImpl::set_inputs_outputs(input_files, output_files);

            //Create the depfiles by appending ".d" to the output files
            {
                const auto &outputs = kvm_[toolchain::Part::Output];
                auto &depfiles = kvm_[toolchain::Part::DepFile];
                depfiles.clear();
                for (const auto &p: outputs)
                {
                    auto fn = p.first+".d";
                    depfiles.emplace_back(fn, "");
                }
            }
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
    };

} } } 

#endif
