#ifndef HEADER_cook_process_command_Link_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Link_hpp_ALREADY_INCLUDED

#include "cook/process/command/CommonImpl.hpp"
#include "cook/process/toolchain/Types.hpp"
#include <memory>

namespace cook { namespace process { namespace command { 

    class Link: public CommonImpl
    {
    public:
        using Ptr = std::shared_ptr<Link>;

        Link(const toolchain::KeyValuesMap &kvm, const toolchain::TranslatorMap &trans, Language language): CommonImpl(kvm, trans, language) {}

        std::string name() const override {return "Link";}
        Type type() const override {return Type::Link;}

        Result process() override {return Result();}

        void add_library(const std::string & name)
        {
            kvm_[toolchain::Part::Library].emplace_back(name, "");
        }
        void add_library_path(const std::filesystem::path & path)
        {
            kvm_[toolchain::Part::LibraryPath].emplace_back(escape_spaces(path.string()), "");
        }
        void add_framework(const std::string & name)
        {
            kvm_[toolchain::Part::Framework].emplace_back(name, "");
        }
        void add_framework_path(const std::filesystem::path & path)
        {
            kvm_[toolchain::Part::FrameworkPath].emplace_back(escape_spaces(path.string()), "");
        }
        void add_export(const std::filesystem::path & path)
        {
            kvm_[toolchain::Part::Export].emplace_back(escape_spaces(path.string()), "");
        }
    };

} } } 

#endif
