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
            auto &trans = *trans_;
        }

        bool set_brand(const std::string &brand) override
        {
            MSS_BEGIN(bool, "");
            auto &trans = *trans_;
            if (false) {}
            else if (brand == "gcc" || brand == "clang")
            {
                if (false) {}
                else if (brand == "gcc")
                    switch (language_)
                    {
                        case Language::C: trans[Part::Cli] = [](const std::string &, const std::string &){return "gcc";}; break;
                        case Language::CXX: trans[Part::Cli] = [](const std::string &, const std::string &){return "g++";}; break;
                    }
                else if (brand == "clang")
                    switch (language_)
                    {
                        case Language::C: trans[Part::Cli] = [](const std::string &, const std::string &){return "clang";}; break;
                        case Language::CXX: trans[Part::Cli] = [](const std::string &, const std::string &){return "clang++";}; break;
                    }

                trans[Part::Pre] = [](const std::string &k, const std::string &v)
                {
                    if (v.empty())
                        return k;
                    return k+"="+v;
                };
                trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};
                trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"-o "}+k;};
                trans[Part::DepFile] = [](const std::string &k, const std::string &v){return std::string{"-MMD -MF "}+k;};
                trans[Part::Define] = [](const std::string &k, const std::string &v)
                {
                    if (v.empty())
                        return std::string{"-D"}+k;
                    return std::string{"-D"}+k+"="+v;
                };
                trans[Part::IncludePath] = [](const std::string &k, const std::string &v){return std::string{"-I "}+k;};
                trans[Part::ForceInclude] = [](const std::string &k, const std::string &v){return std::string{"-include "}+k;};

                if (language_ == Language::CXX)
                    kvm_[Part::Pre].emplace_back("-std", "c++17");
                kvm_[Part::Pre].emplace_back("-c", "");
                bool debug = false;
                if (debug)
                {
                    kvm_[Part::Pre].emplace_back("-g", "");
                }
                else
                {
                    kvm_[Part::Pre].emplace_back("-O3", "");
                    kvm_[Part::Define].emplace_back("NDEBUG", "");
                }
            }
            else if (brand == "msvc")
            {
                trans[Part::Cli] = [](const std::string &, const std::string &){return "cl";};
                trans[Part::Pre] = [](const std::string &k, const std::string &v)
                {
                    if (v.empty())
                        return k;
                    return k+":"+v;
                };
                trans[Part::Input] = [](const std::string &k, const std::string &v){return k;};
                trans[Part::Output] = [](const std::string &k, const std::string &v){return std::string{"/Fo:"}+k;};
                /* trans[Part::DepFile] = [](const std::string &k, const std::string &v){return std::string{"-MMD -MF "}+k;}; */
                trans[Part::Define] = [](const std::string &k, const std::string &v)
                {
                    if (v.empty())
                        return std::string{"/D"}+k;
                    return std::string{"/D"}+k+"="+v;
                };
                trans[Part::IncludePath] = [](const std::string &k, const std::string &v){return std::string{"/I"}+k;};
                trans[Part::ForceInclude] = [](const std::string &k, const std::string &v){return std::string{"/FI"}+k;};

                if (language_ == Language::C)
                    kvm_[Part::Pre].emplace_back("/TC", "");
                kvm_[Part::Pre].emplace_back("/std", "c++latest");
                kvm_[Part::Pre].emplace_back("/nologo", "");
                kvm_[Part::Pre].emplace_back("/EHsc", "");
                kvm_[Part::Pre].emplace_back("-c", "");
                bool debug = false;
                if (debug)
                {
                    kvm_[Part::Pre].emplace_back("/Zi", "");
                }
                else
                {
                    kvm_[Part::Pre].emplace_back("/O3", "");
                    kvm_[Part::Define].emplace_back("NDEBUG", "");
                }
            }
            MSS_END();
        }

        bool create(command::Compile::Ptr &ptr) const override
        {
            MSS_BEGIN(bool);
            ptr.reset(new command::Compile(kvm_, trans_));
            MSS_END();
        }

    private:
        const Language language_;
    };

} } } 

#endif
