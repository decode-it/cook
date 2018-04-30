#include "cook/process/toolchain/Manager.hpp"
#include "cook/process/toolchain/Compiler.hpp"
#include "cook/process/toolchain/Archiver.hpp"
#include "cook/process/toolchain/Linker.hpp"

namespace cook { namespace process { namespace toolchain { 

    Interface &Manager::compiler(Language language)
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

    Interface &Manager::archiver()
    {
        if (!archiver_)
        {
            archiver_.reset(new Archiver);
            archiver_->set_brand(brand_);
        }
        return *archiver_;
    }

    Interface &Manager::linker()
    {
        if (!linker_)
        {
            linker_.reset(new Linker);
            linker_->set_brand(brand_);
        }
        return *linker_;
    }

    bool Manager::set_brand(const std::string &brand)
    {
        MSS_BEGIN(bool);
        if (brand != brand_)
        {
            brand_ = brand;
            for (auto &p: compilers_)
            {
                MSS(p.second->set_brand(brand));
            }
            MSS(archiver_->set_brand(brand));
            MSS(linker_->set_brand(brand));
        }
        MSS_END();
    }

} } } 
