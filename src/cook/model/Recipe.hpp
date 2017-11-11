#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Uri.hpp"
#include "gubg/file/System.hpp"
#include "gubg/macro/capture.hpp"
#include "gubg/OnlyOnce.hpp"
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
#define L_CASE(name) case FileType::name: os << #name; break
            L_CASE(Unknown);
            L_CASE(Source);
            L_CASE(Header);
#undef L_CASE
        }
        return os;
    }

    struct Owner
    {
        enum Type {Nobody = 0x00, Me = 0x01, Deps = 0x02, Anybody = 0x03};
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
        std::string uri_hr(bool add_root = true) const {return uri_.str((add_root ? '/' : '\0'), '/', '.');}
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
                else if (value == "executable")
                {
                    update_output_();
                }
                else MSS(false, type_.clear());
            }
            else if (key == "working_directory") { wd_ = value; }
            else if (key == "script_filename") { script_fn_ = value; }
            else if (key == "depends_on") { deps_.insert(value); }
            else if (key == "display_name") { display_name_ = value; }
            MSS_END();
        }

        void add(std::string p_dir, const std::string &pattern)
        {
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
            };
            gubg::file::each_glob(pattern, add_file, dir);
        }

        bool merge(const Recipe &src)
        {
            MSS_BEGIN(bool);
            for (auto p: src.file_per_path_)
            {
                p.second.owner = Owner::Deps;
                file_per_path_.insert(p);
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

        std::set<std::filesystem::path> include_paths() const
        {
            std::set<std::filesystem::path> ips;
            for (const auto &p: file_per_path_)
            {
                const auto &file = p.second;
                switch (file.type)
                {
                    case FileType::Header:
                    case FileType::ForceInclude:
                        ips.insert(file.dir);
                        break;
                }
            }
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
            if (type_ == "executable")
                output_.filename = uri().str('\0', '_', '.');
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
    };

    inline std::ostream &operator<<(std::ostream &os, const Recipe &r)
    {
        r.stream(os);
        return os;
    }

} } 

#endif
