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

        Archive(const toolchain::KeyValuesMap &kvm, const toolchain::TranslatorMapPtr &trans): CommonImpl(kvm, trans) {}

        std::string name() const override {return "Archive";}
        Type type() const override {return Type::Archive;}
        Result process() override {return Result();}
    };

} } } 

#endif
