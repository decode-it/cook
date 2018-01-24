#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/Uri.hpp"
#include "cook/model/toolchain/Types.hpp"
#include "cook/Types.hpp"
#include "gubg/file/System.hpp"
#include "gubg/macro/capture.hpp"
#include "gubg/OnlyOnce.hpp"
#include "gubg/Range.hpp"
#include <set>
#include <map>
#include <ostream>

namespace cook { namespace model { 

class Book;

struct Owner
{
    enum Type {Nobody = 0x00, Me = 0x01, Deps = 0x02, External = 0x04, Anybody = 0x07};
};

struct File : public cook::File
{
    std::filesystem::path path;

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
using IncludePaths = std::map<std::string, Propagation>;

class Recipe
{
public:
    struct Output
    {
        std::filesystem::path filename;
        std::string name;
    };

    Recipe(const Uri &book_uri): uri_(book_uri) {}

    void set_name(const std::string &name)
    {
        uri_.set_name(name);
    }
    std::string name() const {return uri_.name();}

    const std::string &type() const {return type_;}

    const Uri &uri() const {return uri_;}
    std::string uri_hr(bool add_root = true) const {return uri_.str((add_root ? '/' : '\0'), '/');}

    std::string display_name() const
    {
        if (display_name_.empty())
            return name();
        return display_name_;
    }

    std::filesystem::path script_filename() const {return script_fn_;}

    const Output &output() const { return output_; }

    bool set_type(const std::string &value)
    {
        MSS_BEGIN(bool);
        type_ = value;
        if (value.empty()) {}
        else if (value == "executable" || value == "library")
        {
            update_output_();
        }
        else
            MSS(false, type_.clear());

        MSS_END();
    }

    void set_working_directory(const std::string &value)
    {
        wd_ = value;
    }

    void set_script_filename(const std::string &value)
    {
        script_fn_ = value;
    }

    void set_dependency(const std::string &value)
    {
        deps_.emplace(value, nullptr);
    }
    void set_display_name(const std::string &value)
    {
        display_name_ = value;
        update_output_();
    }

    void add(const std::string & p_dir, const std::string & pattern, FileType default_filetype, const FileFilterPredicate & filter)
    {
        std::filesystem::path dir = (p_dir.empty() ? "." : p_dir);
        if (dir.is_relative())
            dir = wd_ / dir;

        auto add_file_wrapper = [&](const std::filesystem::path &fn)
        {
            File f;
            f.path = fn;

            // get the directory
            f.dir = p_dir.empty() ? "." : p_dir;
            if (f.dir.is_relative())
                f.dir = wd_ / f.dir;

            f.rel.clear();
            //We follow both dir and fp from root to file. As soon as they start to differ,
            //we are in the relative part
            {
                auto dir_it = f.dir.begin();
                auto dir_end = f.dir.end();
                for (const auto &part: f.path)
                {
                    if (dir_it != dir_end && part == *dir_it)
                    {
                        ++dir_it;
                        continue;
                    }
                    f.rel /= part;
                }
            }

            f.owner = Owner::Me;


            const auto ext = f.rel.extension();
            if (false) {}
            else if (ext == ".c")   { f.type = FileType::Source; f.language = "c"; }
            else if (ext == ".h")   { f.type = FileType::Header; f.language = "c"; }
            else if (ext == ".cpp") { f.type = FileType::Source; f.language = "c++"; }
            else if (ext == ".hpp") { f.type = FileType::Header; f.language = "c++"; }
            else if (ext == ".asm") { f.type = FileType::Source; f.language = "asm"; }

            if (default_filetype != Unknown)
                f.type = default_filetype;

            if (!filter || filter(f))
                file_per_path_[fn] = std::move(f);

        };
        gubg::file::each_glob(pattern, add_file_wrapper, dir);
    }

    void add_library(const std::string &lib, Owner::Type owner = Owner::External)
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

        for (const auto &info: src.libraries_)
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

        for (const auto &path: src.library_paths_)
            add_library_path(path);

        for (const auto & macro: src.defines_)
            MSS(add_define(macro.first, macro.second, Overwrite::IfSame));

        for(const auto & p : src.include_paths_)
            if (p.second == Propagation::Global)
                add_include_path(p.first, p.second);


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
                add_library(src.output().name, Owner::Me);
                add_library_path("./");
                for (auto p: src.file_per_path_)
                {
                    const auto &file = p.second;
                    switch (file.type)
                    {
                    case FileType::Header:
                    case FileType::Force_Include:
                        //We only take-over headers to have the correct include paths
                        p.second.owner = Owner::Deps;
                        file_per_path_.insert(p);
                        break;
                    }
                }
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
                //We push_front() to have the correct order (gcc is sensitive to this)
                libs.push_front(info.name);
        return libs;
    }
    toolchain::Libraries libraries(int owner) const {return libraries((Owner::Type)owner);}
    LibraryPaths library_paths() const {return library_paths_;}

    template <typename Ftor>
    bool each_dependency(Ftor ftor)
    {
        MSS_BEGIN(bool);
        for (auto &p: deps_)
        {
            MSS(ftor(p.first, p.second));
        }
        MSS_END();
    }

    std::set<std::filesystem::path> include_paths(Owner::Type owner = Owner::Me) const
    {
        std::set<std::filesystem::path> ips_set;
        for (const auto &p: file_per_path_)
        {
            const auto &file = p.second;
            if (file.owner & owner)
                switch (file.type)
                {
                case FileType::Header:
                case FileType::Force_Include:
                    ips_set.insert(file.dir);
                    break;
                }
        }

        for(const auto & p : include_paths_)
            ips_set.insert(p.first);

        return ips_set;
    }

    toolchain::ForceIncludes force_includes() const
    {
        toolchain::ForceIncludes fis;
        for (const auto &p: file_per_path_)
        {
            const auto &file = p.second;
            switch (file.type)
            {
            case FileType::Force_Include:
                fis.push_back(file.rel.string());
                break;
            }
        }
        return fis;
    }

    using Defines = std::map<std::string, std::string>;
    const Defines & defines() const
    {
        return defines_;
    }

    bool add_define(const std::string & macro, Overwrite overwrite)
    {
        MSS_BEGIN(bool);
        MSS(add_define(macro, "", overwrite));
        MSS_END();
    }

    bool add_define(const std::string & macro, const std::string & value, Overwrite overwrite)
    {
        MSS_BEGIN(bool);

        auto it = defines_.find(macro);

        switch(overwrite)
        {
        case Overwrite::Never:  MSS(it == defines_.end()); break;
        case Overwrite::IfSame: MSS(it == defines_.end() || it->second == value); break;
        case Overwrite::Always: break;
        default: MSS(false);
        }

        defines_[macro] = value;

        MSS_END();
    }

    void add_include_path(const std::string & path, Propagation propagation)
    {
        std::filesystem::path dir = path.empty() ? "." : path;
        if (dir.is_relative())
            dir = wd_/dir;

        include_paths_[dir.string()] = propagation;
    }


    void stream(std::ostream &os) const
    {
        os << "Recipe " << name() << ", uri: " << uri_hr() << ", type: " << type_ << ", working directory: " << wd_ << ", nr files: " << file_per_path_.size() << ", nr deps: " << deps_.size() << std::endl;
        for (const auto &p: file_per_path_)
        {
            os << "\tFile: "; p.second.stream(os); os << std::endl;
        }
        for (const auto &p: deps_)
        {
            os << "\tDep: " << p.first << " => ";
            if (!p.second)
                os << "<unresolved recipe>";
            else
                os << (p.second->uri());
            os << std::endl;
        }
    }

private:
    void update_output_()
    {
        if (false) {}
        else if (type_ == "executable")
        {
            output_.filename = uri().str('\0', '.');
        }
        else if (type_ == "library")
        {
            output_.name = uri_hr(false);
        }
    }

    Uri uri_;
    std::string display_name_;
    std::string type_;
    std::filesystem::path wd_;
    std::filesystem::path script_fn_;
    FilePerPath file_per_path_;
    std::map<std::string, Recipe *> deps_;
    Defines defines_;
    Output output_;
    Libraries libraries_;
    LibraryPaths library_paths_;
    IncludePaths include_paths_;
};

inline std::ostream &operator<<(std::ostream &os, const Recipe &r)
{
    r.stream(os);
    return os;
}

} } 

#endif
