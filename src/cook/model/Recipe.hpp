#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Uri.hpp"
#include "cook/model/toolchain/Types.hpp"
#include "gubg/file/System.hpp"
#include "gubg/macro/capture.hpp"
#include "gubg/OnlyOnce.hpp"
#include "gubg/Range.hpp"
#include <set>
#include <map>
#include <ostream>

namespace cook { namespace model { 

    class Book;

    enum class FileType {Unknown, Source, Header, ForceInclude};
    inline std::ostream &operator<<(std::ostream &os, FileType ft)
    {
        switch (ft)
        {
#define L_CASE(name, str) case FileType::name: os << str; break
            L_CASE(Unknown, "unknown");
            L_CASE(Source, "source");
            L_CASE(Header, "header");
            L_CASE(ForceInclude, "force_include");
#undef L_CASE
        }
        return os;
    }

    struct Owner
    {
        enum Type {Nobody = 0x00, Me = 0x01, Deps = 0x02, External = 0x04, Anybody = 0x07};
    };

    struct File
    {
        std::filesystem::path dir;
        std::filesystem::path path;
        FileType type = FileType::Unknown;
        std::string language;
        Owner::Type owner = Owner::Nobody;

        void stream(std::ostream &os) const
        {
            os << C(path)C(dir)C(type)C(language);
        }
    };
    using FilePerPath = std::map<std::filesystem::path, File>;

    struct LibraryInfo
    {
        std::string name;
        Owner::Type owner = Owner::Nobody;
        LibraryInfo() {}
        LibraryInfo(const std::string &name, Owner::Type owner): name(name), owner(owner) {}
    };
    using Libraries = std::vector<LibraryInfo>;
    using LibraryPaths = model::toolchain::LibraryPaths;

    class Recipe
    {
    public:
        struct Output
        {
            std::filesystem::path filename;
        };

        void set_name(const std::string &name)
        {
            name_ = name;
        }
        const std::string &name() const {return name_;}

        const std::string &type() const {return type_;}

        const Uri &uri() const {return uri_;}
        std::string uri_hr(bool add_root = true) const {return uri_.str((add_root ? '/' : '\0'), '/');}
        template <typename Path>
        void set_path(const Path &path)
        {
            uri_.clear();
            gubg::OnlyOnce skip_root;
            for (auto book_ptr: path)
            {
                if (!skip_root())
                    uri_.add_path_part(book_ptr->name());
            }
            uri_.set_name(name());
            update_output_();
        }

        std::string display_name() const
        {
            if (display_name_.empty())
                return uri_hr();
            return display_name_;
        }

        std::filesystem::path script_filename() const {return script_fn_;}

        const Output &output() const {return output_;}

        bool set(const std::string &key, const std::string &value)
        {
            MSS_BEGIN(bool);
            if (false) { }
            else if (key == "type")
            {
                type_ = value;
                if (value.empty()) {}
                else if (value == "executable" || value == "library")
                {
                    update_output_();
                }
                else MSS(false, type_.clear());
            }
            else if (key == "working_directory") { wd_ = value; }
            else if (key == "script_filename") { script_fn_ = value; }
            else if (key == "depends_on") { deps_.insert(value); }
            else if (key == "display_name") { display_name_ = value; }
            else if (key == "library") { add_library(value, Owner::External); }
            MSS_END();
        }

        void add(std::string p_dir, const std::string &pattern, const std::string &option)
        {
            S("");L(C(pattern));
            if (p_dir.empty())
                p_dir = ".";

            std::filesystem::path dir = p_dir;
            if (dir.is_relative())
                dir = wd_ / dir;

            auto add_file = [&](const std::filesystem::path &fp)
            {
                auto &file = file_per_path_[fp];
                file.owner = Owner::Me;
                file.dir = dir;
                file.path = fp;
                const auto ext = fp.extension();
                if (false) {}
                else if (ext == ".c")   {file.type = FileType::Source; file.language = "c";}
                else if (ext == ".h")   {file.type = FileType::Header; file.language = "c";}
                else if (ext == ".cpp") {file.type = FileType::Source; file.language = "c++";}
                else if (ext == ".hpp") {file.type = FileType::Header; file.language = "c++";}
                else if (ext == ".asm") {file.type = FileType::Source; file.language = "asm";}

                if (option.empty()) {}
                else if (option == "header") {file.type = FileType::Header;}
                else if (option == "source") {file.type = FileType::Source;}
                else if (option == "force_include") {file.type = FileType::ForceInclude;}
            };
            gubg::file::each_glob(pattern, add_file, dir);
        }

        void add_library(const std::string &lib, Owner::Type owner)
        {
            if (std::find_if(RANGE(libraries_), [&](const auto &info){return lib == info.name;}) != libraries_.end())
                return;
            libraries_.push_back(LibraryInfo{lib, owner});
        }
        void add_library_path(const std::filesystem::path &path)
        {
            if (std::find_if(RANGE(library_paths_), [&](const auto &p){return path == p;}) != library_paths_.end())
                return;
            library_paths_.push_back(path.string());
        }

        bool merge(const Recipe &src)
        {
            MSS_BEGIN(bool);
            for (const auto &info: libraries_)
            {
                Owner::Type owner = Owner::Nobody;
                switch (info.owner)
                {
                    case Owner::Me:
                    case Owner::Deps:
                        owner = Owner::Deps;
                        break;
                }
                add_library(info.name, info.owner);
            }
            for (const auto &path: library_paths_)
                add_library_path(path);
            if (src.type().empty())
            {
                for (auto p: src.file_per_path_)
                {
                    p.second.owner = Owner::Deps;
                    file_per_path_.insert(p);
                }
            }
            else if (src.type() == "library")
            {
                add_library(src.output().filename.string(), Owner::Me);
                add_library_path("./");
            }
            MSS_END();
        }

        template <typename Ftor>
        bool each_file(Ftor ftor, Owner::Type owner = Owner::Anybody) const
        {
            MSS_BEGIN(bool);
            for (const auto &p: file_per_path_)
            {
                if (p.second.owner & owner)
                    MSS(ftor(p.second));
            }
            MSS_END();
        }

        toolchain::Libraries libraries(Owner::Type owner) const
        {
            toolchain::Libraries libs;
            for (const auto &info: libraries_)
                if (info.owner & owner)
                    libs.push_back(info.name);
            return libs;
        }
        toolchain::Libraries libraries(int owner) const {return libraries((Owner::Type)owner);}
        LibraryPaths library_paths() const {return library_paths_;}

        template <typename Ftor>
        bool each_dependency(Ftor ftor)
        {
            MSS_BEGIN(bool);
            for (const auto &rn: deps_)
            {
                MSS(ftor(rn));
            }
            MSS_END();
        }

        toolchain::IncludePaths include_paths() const
        {
            std::set<std::filesystem::path> ips_set;
            for (const auto &p: file_per_path_)
            {
                const auto &file = p.second;
                switch (file.type)
                {
                    case FileType::Header:
                    case FileType::ForceInclude:
                        ips_set.insert(file.dir);
                        break;
                }
            }
            toolchain::IncludePaths ips{ips_set.size()};
            ips.resize(0);
            for (const auto &ip: ips_set)
                ips.push_back(ip.string());
            return ips;
        }

        using Defines = std::map<std::string, std::string>;
        Defines defines() const
        {
            Defines defs;
            return defs;
        }

        void stream(std::ostream &os) const
        {
            os << "Recipe " << name_ << ", uri: " << uri_hr() << ", type: " << type_ << ", working directory: " << wd_ << ", nr files: " << file_per_path_.size() << ", nr deps: " << deps_.size() << std::endl;
            for (const auto &p: file_per_path_)
            {
                os << "\tFile: "; p.second.stream(os); os << std::endl;
            }
            for (const auto &dep: deps_)
            {
                os << "\tDep: " << dep << std::endl;
            }
        }

    private:
        void update_output_()
        {
            if (false) {}
            else if (type_ == "executable")
                output_.filename = uri().str('\0', '.');
            else if (type_ == "library")
                output_.filename = uri().last_path();
        }

        std::string name_;
        std::string display_name_;
        Uri uri_;
        std::string type_;
        std::filesystem::path wd_;
        std::filesystem::path script_fn_;
        FilePerPath file_per_path_;
        std::set<std::string> deps_;
        Output output_;
        Libraries libraries_;
        LibraryPaths library_paths_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Recipe &r)
    {
        r.stream(os);
        return os;
    }

} } 

#endif
