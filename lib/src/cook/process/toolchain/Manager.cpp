#include "cook/process/toolchain/Manager.hpp"
#include "cook/process/toolchain/Compiler.hpp"
#include "cook/process/toolchain/Archiver.hpp"
#include "cook/process/toolchain/Linker.hpp"
#include "cook/OS.hpp"

namespace cook { namespace process { namespace toolchain { 

    Manager::Manager()
    {
        switch (get_os())
        {
            case OS::Linux: brand_ = "gcc"; break;
            case OS::Windows: brand_ = "msvc"; break;
            case OS::MacOS: brand_ = "clang"; break;
            default: brand_ = "gcc"; break;
        }
    }

    const Interface &Manager::compiler(Language language) const
    {
        auto it = compilers_.find(language);
        if (it == compilers_.end())
        {
            compilers_[language].reset(new Compiler(language));
            compilers_[language]->set_brand(brand_);
            it = compilers_.find(language);
        }
        return *it->second;
    }

    bool Manager::set_brand(const std::string &brand)
    {
        MSS_BEGIN(bool, "");
        if (brand != brand_)
        {
            brand_ = brand;
            for (auto &p: compilers_)
            {
                MSS(p.second->set_brand(brand));
            }
            MSS(archiver_().set_brand(brand));
            MSS(linker_().set_brand(brand));
        }
        MSS_END();
    }

    Interface &Manager::archiver_() const
    {
        if (!archiver_ptr_)
        {
            archiver_ptr_.reset(new Archiver);
            archiver_ptr_->set_brand(brand_);
        }
        return *archiver_ptr_;
    }

    Interface &Manager::linker_() const
    {
        if (!linker_ptr_)
        {
            linker_ptr_.reset(new Linker);
            linker_ptr_->set_brand(brand_);
        }
        return *linker_ptr_;
    }

} } } 
