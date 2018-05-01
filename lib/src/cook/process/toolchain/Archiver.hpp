#ifndef HEADER_cook_process_toolchain_Archiver_hpp_ALREADY_INCLUDED
#define HEADER_cook_process_toolchain_Archiver_hpp_ALREADY_INCLUDED

#include "cook/process/toolchain/Interface.hpp"
#include "cook/OS.hpp"
#include "gubg/mss.hpp"

namespace cook { namespace process { namespace toolchain { 

    class Archiver: public Interface
    {
    public:
        Archiver()
        {
        }

        bool set_brand(const std::string &brand) override
        {
            MSS_BEGIN(bool);
            auto &trans = *trans_;
            if (false) {}
            else if (brand == "gcc" || brand == "clang")
            {
                trans[Part::Cli] = [](const std::string &, const std::string &){return "ar";};
                trans[Part::Pre] = [](const std::string &k, const std::string &v) {return k;};
                trans[Part::Output] = [](const std::string &k, const std::string &v){return k;};
                trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};

                kvm_[Part::Pre].clear();
                switch (get_os())
                {
                    case OS::Linux: kvm_[Part::Pre].emplace_back("crf", ""); break;
                    case OS::MacOS: kvm_[Part::Pre].emplace_back("crs", ""); break;
                }
            }
            else if (brand == "msvc")
            {
                trans[Part::Cli] = [](const std::string &, const std::string &){return "lib";};
                trans[Part::Pre] = [](const std::string &k, const std::string &v) {return k;};
                trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"/OUT:"}+k;};
                trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};
            }
            MSS_END();
        }

        bool create(command::Archive::Ptr &ptr) const override
        {
            MSS_BEGIN(bool);
            ptr.reset(new command::Archive(kvm_, trans_));
            MSS_END();
        }

    private:
    };

} } } 

#endif
