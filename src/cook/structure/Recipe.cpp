#include "cook/structure/Recipe.hpp"
#include "cook/structure/Book.hpp"
#include <sstream>

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
        for (const auto &p: force_includes())
            res.insert(p.second.dir);
        for (const auto &p: include_paths())
            res.insert(p);
    }
    
    void Recipe::add_library(const std::string &library)
    {
        for (const auto &lib: in_.libraries)
            if (lib == library)
                //Already present
                return;
            in_.libraries.push_back(library);
    }
    
    void Recipe::add_local_include_paths()
    {
        for (const auto &p: sources())
            add_include_path(p.second.dir);
        for (const auto &p: headers())
            add_include_path(p.second.dir);
        for (const auto &p: force_includes())
            add_include_path(p.second.dir);
    }

    void Recipe::print() const
    {
        std::cout << "Defines:" << std::endl;
        for (const auto &p: in_.defines)
            std::cout << " * " << p.first << " => " << p.second << std::endl;
        std::cout << "Sources:" << std::endl;
        for (const auto &p: sources_)
            std::cout << " * " << p.first << " => " << p.second << std::endl;
        std::cout << "Headers:" << std::endl;
        for (const auto &p: headers_)
            std::cout << " * " << p.first << " => " << p.second << std::endl;
        std::cout << "Force includes:" << std::endl;
        for (const auto &p: force_includes_)
            std::cout << " * " << p.first << " => " << p.second << std::endl;
    }
    
    std::string Recipe::propose_target_identifier() const
    {
        std::string start_point = !display_name().empty() ? display_name() : uri().string();
        
        // translate the file
        std::ostringstream oss;
        for(char c : start_point)
            if ('c' == '#' || 'c' == ' ')
                oss << "_";
            else
                oss << c;
        return oss.str();
    }
    
    bool Recipe::configure(const Config & config) 
    {
        MSS_BEGIN(bool);
        
        switch(target_type())
        {
            case TargetType::StaticLibrary:
            {
                std::string id = target_identifier();
                std::string lib_name = std::string("lib") + id + std::string(".a");
                
                output_.library_paths.insert("./");
                output_.libraries.push_back(id);
                
                for(const auto & p : headers())
                    output_.include_paths.insert(p.second.dir);
                for(const auto & p : force_includes())
                    output_.include_paths.insert(p.second.dir);
                
                output_.filename = config.build_dir / lib_name;
            }
                break;
                
            case TargetType::Executable:
            {
                std::string id = target_identifier();
                std::string exe_name = id;
                
                output_.filename = config.build_dir / exe_name;
            }
                break;

            case TargetType::Unknown:
            {
                static_cast<TargetConfig &>(output_) = in_;
                break;
            }
                
            default:
                break;
        }
        
        MSS_END();
    }
    
    
    bool Recipe::construct_target(Target & tgt) const
    {
        MSS_BEGIN(bool);
        
        tgt.identifier = target_identifier();
        
        switch(target_type())
        {
            case TargetType::StaticLibrary:
                tgt.link_target = tgt.identifier;
                tgt.filename = std::string("lib") + tgt.link_target + std::string(".a");
                break;
                
            case TargetType::Executable:
                tgt.filename = tgt.identifier;
                break;
                
            default:
                break;
        }
        
        MSS_END();
    }
    
//     void Recipe::merge(const Recipe &rhs)
//     {
//         defines_.insert(rhs.defines_.begin(), rhs.defines_.end());
//         include_paths_.insert(rhs.include_paths_.begin(), rhs.include_paths_.end());
//         library_paths_.insert(rhs.library_paths_.begin(), rhs.library_paths_.end());
//         
//         for (const auto &lib: rhs.libraries_)
//             add_library(lib);
//     }
       
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
