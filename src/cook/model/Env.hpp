#ifndef HEADER_cook_model_Env_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Env_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"
#include <string>
#include <map>

namespace cook { namespace model { 

    class Env
    {
    public:
        void set_dir(std::string name, std::string dir) {dirs_[name] = dir;}
        const std::filesystem::path dir(const std::string &name) const
	{
		auto it = dirs_.find(name);
		if (it == dirs_.end())
			//Default is local dir
			return "./";
		return it->second;
	}

    private:
	using Name = std::string;
	using Dir = std::string;
	std::map<Name, Dir> dirs_;
    };

} } 

#endif
