#include "cook/chai/Toolchain.hpp"

namespace cook { namespace chai { 

    Toolchain::Toolchain(process::toolchain::Manager * manager)
        : manager_(manager)
    {
    }

    bool Toolchain::has_config(const std::string & key) const
    {
        return manager_->has_config(key);
    }
        
    std::string Toolchain::config_value(const std::string & key) const
    {
        return manager_->config_value(key);
    }
        
    void Toolchain::set_config_value(const std::string & key, const std::string & value)
    {
        manager_->configure(key, value);
    }

} }
