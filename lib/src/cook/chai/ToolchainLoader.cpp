#include "cook/chai/ToolchainLoader.hpp"
#include "cook/chai/toolchain/serialize/GCC.hpp"
#include "cook/chai/toolchain/serialize/MSVC.hpp"
#include "cook/util/File.hpp"
#include "cook/OS.hpp"
#include <fstream>

namespace cook { namespace chai {

    namespace {

        void serialize_gcc(std::ostream & oss) 
        { 
            std::map<Language, std::string> compilers = { {Language::C, "gcc"}, {Language::CXX, "g++"}, {Language::ASM, "gcc"} };
            toolchain::serialize::gcc_config(oss, compilers, "g++");
        }

        void serialize_clang(std::ostream & oss)
        {
            std::map<Language, std::string> compilers = { {Language::C, "clang"}, {Language::CXX, "clang++"}, {Language::ASM, "clang"} };
            toolchain::serialize::gcc_config(oss, compilers, "clang");
        }

        void serialize_msvc(std::ostream & oss) 
        {
            toolchain::serialize::msvc_config(oss);
        }

        bool is_file(const std::filesystem::path & p, const std::string & name, std::filesystem::path & fn)
        {
            fn = gubg::filesystem::combine(p, name);
            return std::filesystem::exists(fn) && std::filesystem::is_regular_file(fn);
        }
    }

    ToolchainLoader::ToolchainLoader()
    {   
        known_serializers_["gcc"] = serialize_gcc;
        known_serializers_["clang"] = serialize_clang;
        known_serializers_["msvc"] = serialize_msvc;

        switch(get_os())
        {
            case OS::Linux:
                known_serializers_["default"] = known_serializers_["gcc"];
                break;

            case OS::Windows:
                known_serializers_["default"] = known_serializers_["msvc"];
                break;

            case OS::MacOS:
                known_serializers_["default"] = known_serializers_["clang"];
                break;
        }

        include_paths_.push_back("./");
    }

    void ToolchainLoader::add_include_path(const std::filesystem::path & path) 
    {
        for(const std::filesystem::path & p : include_paths_)
            if (p == path)
                return;

        include_paths_.push_back(path);
    }

    Result ToolchainLoader::load(const std::string & name, std::filesystem::path & fn) const
    {
        MSS_BEGIN(Result);

        bool found = false;

        for(auto it = include_paths_.begin(); it != include_paths_.end() && !found; ++it)
        {
            // find the file <name> in any of the include paths
            found = is_file(*it, name, fn);

            // find the file <name>.chai in the include path
            if (!found)
                found = is_file(*it, name + ".chai", fn);
        }


        // try if we can write it to stream
        if (!found)
        {
            auto it = known_serializers_.find(name);
            MSG_MSS(it != known_serializers_.end(), Error, "Cannot find file " << name << " and I do no know that toolchain");

            found = true;
            fn = name + ".chai";
            
            MSS_RC << MESSAGE(Info, "Generating toolchain file " << fn);

            std::ofstream ofs;
            MSS(util::open_file(name + ".chai", ofs));
            it->second(ofs);
        }

        MSS(found);

        MSS_END();
    }

} }


