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

        Link(toolchain::Element::Ptr ptr)
            : CommonImpl(ptr)
        {
        }

        std::string name() const override {return "Link";}
        Type type() const override {return Type::Link;}

        Result process() override {return Result();}
        
        virtual bool process_ingredient(const LanguageTypePair& ltp, const ingredient::File& file) override
        { 
            if (false)
            {
            }
            else if (ltp.language == Language::Binary && ltp.type == cook::Type::Object)
            {
                return true;
            }
            else if (ltp.language == Language::Binary && ltp.type == cook::Type::Dependency)
            {
            }
            else if (ltp.language == Language::Binary && ltp.type == cook::Type::LibraryPath)
            {
                add_library_path_(file.dir());
            }
            else if (ltp.language == Language::Binary && ltp.type == cook::Type::FrameworkPath)
            {
                add_framework_path_(file.dir());
            }
            else if(ltp.language == Language::Definition && ltp.type == cook::Type::Export)
            {
                add_export_(file.key());
                return true;
            }
            else
            {
                return CommonImpl::process_ingredient(ltp, file);
            }
                
            return false; 
        }

        virtual bool process_ingredient(const LanguageTypePair& ltp, const ingredient::KeyValue& kv) override
        {
            if (false)
            {
            }
            else if (ltp.language == Language::Binary && ltp.type == cook::Type::Library)
            {
                add_library_(kv.key());
            }
            else if (ltp.language == Language::Binary && ltp.type == cook::Type::Framework)
            {
                add_framework_(kv.key());
            }
            else
            {
                return CommonImpl::process_ingredient(ltp, kv);
            }
            return false; 
        }

    private:
        void add_library_(const std::string & name)
        {
            kvm_[toolchain::Part::Library].emplace_back(name, "");
        }
        void add_library_path_(const std::filesystem::path & path)
        {
            kvm_[toolchain::Part::LibraryPath].emplace_back(escape_spaces(path.string()), "");
        }
        void add_framework_(const std::string & name)
        {
            kvm_[toolchain::Part::Framework].emplace_back(name, "");
        }
        void add_framework_path_(const std::filesystem::path & path)
        {
            kvm_[toolchain::Part::FrameworkPath].emplace_back(escape_spaces(path.string()), "");
        }
        void add_export_(const std::filesystem::path & path)
        {
            kvm_[toolchain::Part::Export].emplace_back(escape_spaces(path.string()), "");
        }
    };

} } } 

#endif
