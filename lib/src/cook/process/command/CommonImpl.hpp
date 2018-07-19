#ifndef HEADER_cook_process_command_CommonImpl_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_command_CommonImpl_hpp_ALREADY_INCLUDED

#include "cook/process/command/Interface.hpp"
#include "cook/process/toolchain/Types.hpp"
#include "cook/Language.hpp"
#include "gubg/OnlyOnce.hpp"

namespace cook { namespace process { namespace command { 

    class CommonImpl: public Interface
    {
    public:
        CommonImpl(const toolchain::KeyValuesMap &kvm, const toolchain::TranslatorMapPtr &trans, Language language);

        void set_inputs_outputs(const Filenames & input_files, const Filenames & output_files) override;
        bool stream_part(std::ostream & os, toolchain::Part part, const toolchain::Translator *trans_ptr = nullptr) const override;
        void stream_command(std::ostream & os) const override;

    protected:
        toolchain::KeyValuesMap kvm_;
        toolchain::TranslatorMapPtr trans_;
        const Language language_;
    };

} } } 

#endif
