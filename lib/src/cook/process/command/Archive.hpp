#ifndef HEADER_cook_process_command_Archive_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_Archive_hpp_ALREADY_INCLUDED

#include "cook/process/command/CommonImpl.hpp"
#include "cook/process/toolchain/Types.hpp"
#include <memory>

namespace cook { namespace process { namespace command { 

    class Archive: public CommonImpl
    {
    public:
        using Ptr = std::shared_ptr<Archive>;

        Archive(toolchain::Element::Ptr ptr) : CommonImpl(ptr) {}
        
        virtual bool process_ingredient(const LanguageTypePair& ltp, const ingredient::File& file) override
        { 
            if (false)
            {
            }
            else if (ltp.language == Language::Binary && ltp.type == cook::Type::Object)
            {
                return true;
            }
            else
            {
                return CommonImpl::process_ingredient(ltp, file);
            }
                
            return false; 
        }
        
        bool delete_before_build() const override { return true; }

        std::string name() const override {return "Archive";}
        Type type() const override {return Type::Archive;}
        Result process() override {return Result();}
    };

} } } 

#endif
