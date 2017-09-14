#ifndef HEADER_cook_structure_Description_hpp_ALREADY_INCLUDED
#define HEADER_cook_structure_Description_hpp_ALREADY_INCLUDED

#include "cook/structure/Alias.hpp"
#include "gubg/debug.hpp"
#include "gubg/file/System.hpp"
#include <map>
#include <set>

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
    using FileInfo = std::map<std::filesystem::path, file::Info>;
    using IncludePaths = std::set<std::filesystem::path>;
    using Defines = std::map<std::string, std::string>;
    using LibraryPaths = std::set<std::filesystem::path>;
    //Order is important: we cannot use std::set
    using Libraries = std::vector<std::string>;
    using Dependencies = std::set<std::string>;

    class Recipe
    {
        private:
            static constexpr const char *logns = "structure::Recipe";

        public:
            Recipe(const Alias &alias): alias_(alias) {}

            void set_base(const std::filesystem::path &dir) {base_ = dir;}
            const std::filesystem::path &base() const {return base_;}

            const Alias &alias() const {return alias_;}

            const FileInfo &sources() const {return sources_;}
            const FileInfo &headers() const {return headers_;}
            const IncludePaths &include_paths() const {return include_paths_;}
            const Defines &defines() const {return defines_;}
            const LibraryPaths &library_paths() const {return library_paths_;}
            const Libraries &libraries() const {return libraries_;}
            const Dependencies &dependencies() const {return dependencies_;}

            void get_all_include_paths(IncludePaths &res) const;

            //Methods exposed to chai
            void add_source(const std::string &dir, const std::string &pattern) { add_(sources_, file::Source, dir, pattern); }
            void add_header(const std::string &dir, const std::string &pattern) { add_(headers_, file::Header, dir, pattern); }
            void add_include_path(const std::string &dir) { include_paths_.insert(dir); }
            void add_define_1(const std::string &name) { defines_[name] = ""; }
            void add_define_2(const std::string &name, const std::string &value) { defines_[name] = value; }
            void add_library_path(const std::string &dir) { library_paths_.insert(dir); }
            void add_library(const std::string &library);
            void depends_on(const std::string &alias) { dependencies_.insert(alias); }
            void print() const;
            
            void merge(const Recipe &rhs);
            

        private:
            void add_(FileInfo &dst, file::Type type, const std::string &subdir, const std::string &pattern);
            

            const Alias alias_;
            std::filesystem::path base_;
            FileInfo sources_;
            FileInfo headers_;
            IncludePaths include_paths_;
            Defines defines_;
            LibraryPaths library_paths_;
            Libraries libraries_;
            Dependencies dependencies_;
    };

} } 

#endif
