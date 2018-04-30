#ifndef HEADER_cook_process_toolchain_Compiler_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Compiler_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Interface.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace process { namespace toolchain { 

    class Compiler: public Interface
    {
    public:
        Compiler(Language language): language_(language)
        {
        }

        bool set_brand(const std::string &brand) override
        {
            MSS_BEGIN(bool);
            auto &trans = *trans_;
            if (false) {}
            else if (brand == "gcc")
            {
                switch (language_)
                {
                    case Language::C:
                        trans[Part::Cli] = [](const std::string &, const std::string &){return "gcc";};
                        break;
                    case Language::CXX:
                        trans[Part::Cli] = [](const std::string &, const std::string &){return "g++";};
                        break;
                }
            }
            MSS_END();
        }

    private:
        const Language language_;
    };

} } } 

#endif
