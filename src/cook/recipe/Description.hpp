#ifndef HEADER_cook_recipe_Description_hpp_ALREADY_INCLUDED
#define HEADER_cook_recipe_Description_hpp_ALREADY_INCLUDED

#include "cook/recipe/Alias.hpp"
#include "gubg/debug.hpp"
#include "gubg/file/System.hpp"
#include <map>
#include <set>

namespace cook { namespace recipe { 

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
    using Libraries = std::set<std::string>;

    class Description
    {
        private:
            static constexpr const char *logns = "recipe::Description";

        public:

            Description(const Alias &alias): alias_(alias) {}

            const Alias &alias() const {return alias_;}

            const FileInfo &sources() const {return sources_;}
            const FileInfo &headers() const {return headers_;}
            const Libraries &libraries() const {return libraries_;}

            //Methods exposed to chai
            void add_source(const std::string &dir, const std::string &pattern) { add_(sources_, file::Source, dir, pattern); }
            void add_header(const std::string &dir, const std::string &pattern) { add_(headers_, file::Header, dir, pattern); }
            void add_library(const std::string &library)
            {
                libraries_.insert(library);
            }
            void print() const
            {
                std::cout << "Sources:" << std::endl;
                for (const auto &p: sources_)
                    std::cout << " * " << p.first << " => " << p.second << std::endl;
                std::cout << "Headers:" << std::endl;
                for (const auto &p: headers_)
                    std::cout << " * " << p.first << " => " << p.second << std::endl;
            }

        private:
            void add_(FileInfo &dst, file::Type type, const std::string &dir, const std::string &pattern)
            {
                S("add_");
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
            FileInfo sources_;
            FileInfo headers_;
            Libraries libraries_;
    };

} } 

#endif
