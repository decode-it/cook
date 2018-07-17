#include "cook/process/toolchain/Loader.hpp"
#include "cook/process/toolchain/serialize/GCC.hpp"
#include "cook/process/toolchain/serialize/MSVC.hpp"
#include "cook/util/File.hpp"
#include "cook/OS.hpp"
#include "cook/Version.hpp"
#include <fstream>
#include <cstdlib>

namespace cook { namespace process { namespace toolchain {

    namespace {

        const std::vector<Language> languages = {Language::C, Language::CXX, Language::ASM, Language::ObjectiveC, Language::ObjectiveCXX};

        void serialize_gcc(std::ostream & oss) 
        { 
            serialize::gcc_config(oss, serialize::GCCVariant::Genuine, languages, "gcc", "ar", "g++");
        }

        void serialize_clang(std::ostream & oss)
        {
            serialize::gcc_config(oss, serialize::GCCVariant::Clang, languages, "clang", "ar", "clang++");
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

    bool Loader::translate_(const std::string & base, std::string & translated) const
    {
        if (base != "default")
            return false;

        switch(get_os())
        {
            case OS::Linux:
                translated = "gcc";
                break;

            case OS::Windows:
                translated = "msvc";
                break;

            case OS::MacOS:
                translated = "clang";
                break;

            default:
                return false;
        }
        return true;
    }

    Loader::Loader()
    {   
        known_serializers_["gcc"] = serialize_gcc;
        known_serializers_["clang"] = serialize_clang;
        known_serializers_["msvc"] = serialize_msvc;

        include_paths_.push_back("./");
        include_paths_.push_back(get_cook_home_path());
    }

    void Loader::add_include_path(const std::filesystem::path & path) 
    {
        for(const std::filesystem::path & p : include_paths_)
            if (p == path)
                return;

        {
            auto second_to_last = include_paths_.end();
            --second_to_last;
            include_paths_.insert(second_to_last, path);
        }
    }

    std::filesystem::path Loader::get_cook_home_path()
    {
        std::filesystem::path fn;

        switch(get_os())
        {
            case OS::Windows:
                {
                    const char * drive = getenv("HOMEDRIVE");
                    const char * path = getenv("HOMEPATH");

                    if (!drive || !path)
                        break;

                    fn = std::filesystem::path(drive) / path / "AppData" / "Local" / "cook" / version() / "toolchain";
                }
                break;

            case OS::MacOS:
            case OS::Linux:
                {
                    const char * home = getenv("HOME");
                    if (!home)
                        break;

                    fn = std::filesystem::path(home) / ".config" / "cook" / version() / "toolchain";
                }
                break;

            default:
                break;
        }

        return fn;
    }


    Result Loader::load(const std::string & name, std::filesystem::path & fn) const
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

        {
            std::string translated;
            if (translate_(name, translated))
                return load(translated, fn);
        }   

        // try if we can write it to stream
        if (!found)
        {
            auto it = known_serializers_.find(name);
            MSG_MSS(it != known_serializers_.end(), Error, "Cannot find file " << name << " and I do no know that toolchain");

            
            found = true;
            fn = get_cook_home_path() / (name + ".chai");
            
            MSS_RC << MESSAGE(Info, "Generating toolchain file " << fn);

            std::ofstream ofs;
            MSS(util::open_file(fn, ofs));
            it->second(ofs);
        }

        MSS(found);

        MSS_END();
    }

} } }


