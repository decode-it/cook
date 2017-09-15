#include "cook/structure/Recipe.hpp"
#include "cook/structure/Book.hpp"

namespace cook { namespace structure { 
    
    Recipe::Recipe(const std::filesystem::path & script_fn, const Tag & tag, Book * book)
                :  Element(Type::Recipe, script_fn, tag, book),
                   book_(book)
    {
        
    }
       
    void Recipe::get_all_include_paths(IncludePaths &res) const
    {
        for (const auto &p: sources())
            res.insert(p.second.dir);
        for (const auto &p: headers())
            res.insert(p.second.dir);
        for (const auto &p: include_paths())
            res.insert(p);
    }
    
    void Recipe::add_library(const std::string &library)
    {
        for (const auto &lib: libraries_)
            if (lib == library)
                //Already present
                return;
            libraries_.push_back(library);
    }
    
    void Recipe::add_local_include_paths()
    {
        for (const auto &p: sources())
            include_paths_.insert(p.second.dir);
        for (const auto &p: headers())
            include_paths_.insert(p.second.dir);
    }
    
    void Recipe::print() const
    {
        std::cout << "Defines:" << std::endl;
        for (const auto &p: defines_)
            std::cout << " * " << p.first << " => " << p.second << std::endl;
        std::cout << "Sources:" << std::endl;
        for (const auto &p: sources_)
            std::cout << " * " << p.first << " => " << p.second << std::endl;
        std::cout << "Headers:" << std::endl;
        for (const auto &p: headers_)
            std::cout << " * " << p.first << " => " << p.second << std::endl;
    }
    
    bool Recipe::get_target_filename(std::string & tgt) const
    {
        tgt = uri().string();;
        
        switch (target_type())
        {
            case TargetType::Executable:                        break;
            case TargetType::StaticLibrary:     tgt += ".a";    break;
            default:
                return false;
        }
        return true;
    }
    
    void Recipe::merge(const Recipe &rhs)
    {
        defines_.insert(rhs.defines_.begin(), rhs.defines_.end());
        include_paths_.insert(rhs.include_paths_.begin(), rhs.include_paths_.end());
        library_paths_.insert(rhs.library_paths_.begin(), rhs.library_paths_.end());
        
        for (const auto &lib: rhs.libraries_)
            add_library(lib);
    }
       
    std::string Recipe::string() const
    {
        std::ostringstream oss;
        oss << uri();
        return oss.str();
    }
    
    void Recipe::add_(FileInfo &dst, file::Type type, const std::string &subdir, const std::string &pattern)
    {
        S("add_");
        std::filesystem::path dir = base(); dir /= subdir;
        L(C(subdir)C(dir)C(pattern));
        auto add_file = [&](const std::filesystem::path &fp)
        {
            L(C(fp));
            /* std::filesystem::path fp(dir); fp /= pattern; */
            auto &info = dst[fp];
            info.dir = dir;
            info.rel = pattern;
            info.type = type;
        };
        gubg::file::each_glob(pattern, add_file, dir);
    }
} } 
