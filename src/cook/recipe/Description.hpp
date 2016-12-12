#ifndef HEADER_cook_recipe_Description_hpp_ALREADY_INCLUDED
#define HEADER_cook_recipe_Description_hpp_ALREADY_INCLUDED

#include "cook/recipe/Alias.hpp"
#include "gubg/file/Name.hpp"
#include "gubg/debug.hpp"
#include <map>

namespace cook { namespace recipe { 

    namespace file { 
        enum Type
        {
            Unknown = -1, Source, Header, ForceInclude,
        };
        struct Info
        {
            gubg::file::Name dir;
            gubg::file::Name rel;
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

    class Description
    {
        private:
            static constexpr const char *logns = "recipe::Description";

        public:
            Description(const Alias &alias): alias_(alias) {}

            const Alias &alias() const {return alias_;}

            void add_source(const std::string &dir, const std::string &pattern) { add_(sources_, file::Source, dir, pattern); }
            void add_header(const std::string &dir, const std::string &pattern) { add_(headers_, file::Header, dir, pattern); }
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
            using FileInfo = std::map<gubg::file::Name, file::Info>;

            void add_(FileInfo &dst, file::Type type, const std::string &dir, const std::string &pattern)
            {
                gubg::file::Name fp(dir); fp /= pattern;
                auto &info = dst[fp];
                info.dir = dir;
                info.rel = pattern;
                info.type = type;
            }

            const Alias alias_;
            FileInfo sources_;
            FileInfo headers_;
    };

} } 

#endif
