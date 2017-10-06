#ifndef HEADER_cook_structure_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_structure_Recipe_hpp_ALREADY_INCLUDED

#include "cook/structure/Element.hpp"
#include "cook/structure/TargetConfig.hpp"
#include "cook/structure/Uri.hpp"
#include "cook/structure/Tag.hpp"
#include "gubg/debug.hpp"
#include "gubg/file/System.hpp"
#include <map>
#include <set>
#include <vector>

namespace cook { namespace structure { 

    namespace file { 
        enum Type
        {
            Unknown = -1, Source, Header, ForceInclude,
        };
        struct Info
        {
            std::filesystem::path dir;
            std::filesystem::path rel;
            Type type;

            void stream(std::ostream &os) const
            {
                os << C(dir)C(rel)C(type, int);
            }
        };
        inline std::ostream &operator<<(std::ostream &os, const Info &info)
        {
            info.stream(os);
            return os;
        }
    } 
    
    enum class TargetType
    {
        Unknown,
        Executable,
        StaticLibrary
    };
    
    inline std::ostream & operator<<(std::ostream & str, TargetType tgt)
    {
        switch(tgt)
        {
            case TargetType::Executable: str << "Executable"; break;
            case TargetType::StaticLibrary: str << "StaticLibrary"; break; 
            default: str << "Unknown";
        }
        
        return str;
    }
    
    
    using FileInfo          = std::map<std::filesystem::path, file::Info>;
    using Dependencies      = std::set<Uri>;
    
    struct Book;
    
    
    struct Config
    {
        std::filesystem::path build_dir;
    };
    
    struct RecipeOutput : public TargetConfig
    {
        std::filesystem::path filename;
    };
    
    
    struct Target
    {
        std::string filename;
        std::string link_target;
        std::string identifier;
    };

    class Recipe : public Element
    {
        using RequiredRecipes   = std::set<Recipe *>;
        
        private:
            static constexpr const char *logns = "structure::Recipe";

        public:
            Recipe(const std::filesystem::path & script_fn, const Tag & tag, Book * book);
                        
            std::filesystem::path base() const                                                  { return script_filename().parent_path(); }
            const Tag & tag() const                                                             { return uri().back(); }
            const Book & book() const                                                           { return *book_; }
            Book & book()                                                                       { return *book_; }
            TargetType target_type() const                                                      { return target_type_; }
            void set_target_type(TargetType target_type)                                        { target_type_ = target_type; }

            const FileInfo &sources() const                                                     { return sources_; }
            const FileInfo &headers() const                                                     { return headers_; }
            const IncludePaths &include_paths() const                                           { return in_.include_paths; }
            const Defines &defines() const                                                      { return in_.defines; }
            const LibraryPaths &library_paths() const                                           { return in_.library_paths; }
            const Libraries &libraries() const                                                  { return in_.libraries; }
            const Dependencies &dependencies() const                                            { return dependencies_; }
            
            
            void add_local_include_paths();
                       
            std::string propose_target_identifier() const;
            void set_target_identifier(const std::string & identifier)                          { identifier_ = identifier; }
            const std::string & target_identifier() const                                       { return identifier_; }
            bool construct_target(Target & target) const;
                        
            void get_all_include_paths(IncludePaths &res) const;

            void add_source(const std::string &dir, const std::string &pattern)                 { add_(sources_, file::Source, dir, pattern); }
            void add_header(const std::string &dir, const std::string &pattern)                 { add_(headers_, file::Header, dir, pattern); }
            void add_include_path(const std::string &dir)                                       { in_.include_paths.insert(dir); }
            void add_define(const std::string &name, const std::string &value = std::string())  { in_.defines[name] = value; }
            void add_library_path(const std::string &dir)                                       { in_.library_paths.insert(dir); }
            void depends_on(const Uri & uri)                                                    { dependencies_.insert(uri); }
            const RequiredRecipes & required_recipes() const                                    { return required_recipes_; }
            void add_library(const std::string &library);
            void print() const;
            
            const TargetConfig & input_config() const                                           { return in_; }
            
//             void merge(const Recipe &rhs);
            
            void add_required_recipe(Recipe * recipe)                                           { if(recipe != nullptr) required_recipes_.insert(recipe); }
            std::string string() const;
            
            bool configure(const Config & config);
            const RecipeOutput & output() const                                                 { return output_; }

        private:
            Recipe(const Recipe &) = delete;
            Recipe & operator=(const Recipe &) = delete;
            Recipe(Recipe &&) = delete;
            Recipe operator=(Recipe &&) = delete;
            
            Book * const book_;
            TargetType target_type_ = TargetType::Unknown;
            
            void add_(FileInfo &dst, file::Type type, const std::string &subdir, const std::string &pattern);
            
            // info directly settable from chai script
            FileInfo sources_;
            FileInfo headers_;
            TargetConfig in_;
            Dependencies dependencies_;
            
            // structural information set after first run
            RequiredRecipes required_recipes_;
            std::string identifier_;
            
            
            RecipeOutput output_;
    };

} } 

#endif

