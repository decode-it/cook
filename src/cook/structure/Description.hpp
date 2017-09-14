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

    class Description
    {
        private:
            static constexpr const char *logns = "structure::Description";

        public:
            Description(const Alias &alias): alias_(alias) {}

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

            void get_all_include_paths(IncludePaths &res) const
            {
                for (const auto &p: sources())
                    res.insert(p.second.dir);
                for (const auto &p: headers())
                    res.insert(p.second.dir);
                for (const auto &p: include_paths())
                    res.insert(p);
            }

            //Methods exposed to chai
            void add_source(const std::string &dir, const std::string &pattern) { add_(sources_, file::Source, dir, pattern); }
            void add_header(const std::string &dir, const std::string &pattern) { add_(headers_, file::Header, dir, pattern); }
            void add_include_path(const std::string &dir) { include_paths_.insert(dir); }
            void add_define_1(const std::string &name) { defines_[name] = ""; }
            void add_define_2(const std::string &name, const std::string &value) { defines_[name] = value; }
            void add_library_path(const std::string &dir) { library_paths_.insert(dir); }
            void add_library(const std::string &library)
            {
                for (const auto &lib: libraries_)
                    if (lib == library)
                        //Already present
                        return;
                libraries_.push_back(library);
            }
            void depends_on(const std::string &alias) { dependencies_.insert(alias); }
            void print() const
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

            void merge(const Description &rhs)
            {
                sources_.insert(rhs.sources_.begin(), rhs.sources_.end());
                headers_.insert(rhs.headers_.begin(), rhs.headers_.end());
                defines_.insert(rhs.defines_.begin(), rhs.defines_.end());
                include_paths_.insert(rhs.include_paths_.begin(), rhs.include_paths_.end());
                library_paths_.insert(rhs.library_paths_.begin(), rhs.library_paths_.end());
                for (const auto &lib: rhs.libraries_)
                    add_library(lib);
            }

        private:
            void add_(FileInfo &dst, file::Type type, const std::string &subdir, const std::string &pattern)
            {
                S("add_");
                std::filesystem::path dir = base_; dir /= subdir;
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
