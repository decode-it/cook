#include "cook/model/Recipe.hpp"
#include "cook/model/object/File.hpp"
#include "cook/model/object/Define.hpp"
#include "cook/model/object/Path.hpp"

namespace cook { namespace model {


Recipe::Recipe(const Uri &book_uri)
    : uri_(book_uri)
{
}

void Recipe::set_name(const std::string &name)
{
    uri_.set_name(name);
}

std::string Recipe::name() const
{
    return uri_.name();
}

const std::string & Recipe::type() const
{
    return type_;
}

const Uri & Recipe::uri() const
{
    return uri_;
}

std::string Recipe::uri_hr(bool add_root) const
{
    return uri_.str((add_root ? '/' : '\0'), '/');
}

std::string Recipe::display_name() const
{
    if (display_name_.empty())
        return name();
    return display_name_;
}

const Recipe::Output & Recipe::output() const
{
    return output_;
}

bool Recipe::set_type(const std::string &value)
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

const std::filesystem::path & Recipe::working_directory() const
{
    return wd_;
}

void Recipe::set_working_directory(const std::string &value)
{
    wd_ = value;
}

void Recipe::set_dependency(const std::string &value)
{
    deps_.emplace(value, nullptr);
}
void Recipe::set_display_name(const std::string &value)
{
    display_name_ = value;
    update_output_();
}

void Recipe::add(const std::string & p_dir, const std::string & pattern, cook_Flags_t default_flags, const FileFilterPredicate & filter)
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

        f.flags = default_flags;

        const auto ext = f.rel.extension();
        if (false) {}
        else if (ext == ".c")   { f.type = cook_FileType_Source; f.language = "c"; }
        else if (ext == ".h")   { f.type = cook_FileType_Header; f.language = "c"; }
        else if (ext == ".cpp") { f.type = cook_FileType_Source; f.language = "c++"; }
        else if (ext == ".hpp") { f.type = cook_FileType_Header; f.language = "c++"; }
        else if (ext == ".asm") { f.type = cook_FileType_Source; f.language = "asm"; }

        if (f.type == cook_FileType_t{})
            f.type = cook_FileType_Unknown;

        // do we accept the file ?
        if (!filter || filter(f))
        {
            // and set the include directory for force includes or header files
            if (is_set(f.flags, cook_Flags_ForceInclude) || (f.type == cook_FileType_Header))
            {
                switch(f.propagation)
                {
                case cook_Propagation_Default:
                case cook_Propagation_Public:
                    add_include_path_(f.dir, Propagation::Global);
                    break;
                default:
                    add_include_path_(f.dir, Propagation::Local);
                    break;
                }
            }


            {
                object::Type t = object::Type::UnknownFile;
                if (false) {}
                else if (f.type == cook_FileType_Header) { t = object::Type::Header; }
                else if (f.type == cook_FileType_Source) { t = object::Type::Source ;}
                else if (f.type >= cook_FileType_UserDefined) { t = static_cast<object::Type>(f.type); }

                object::File p = object::File(t, f.dir, f.rel);
                p.flags = f.flags;
                p.language = f.language;
                p.owner = f.owner;
                p.file_type = f.type;

                // add the file anyway
                files_.insert(p, cook_Overwrite_Never);

                // should it be a force include ?
                if (is_set(f.flags, cook_Flags_ForceInclude))
                    files_.insert(object::Type::ForceInclude, p, cook_Overwrite_Never);
            }

            // add it to the list
            file_per_path_[fn] = std::move(f);
        }

    };
    gubg::file::each_glob(pattern, add_file_wrapper, dir);
}

void Recipe::add_library(const std::string &lib, Propagation propagation, Owner::Type owner)
{
    object::Path p = object::Path(object::Type::Library, lib);
    p.owner = owner;
    p.propagation = (propagation == Propagation::Global ? cook_Propagation_Public : cook_Propagation_Private);

    paths_.insert(p, cook_Overwrite_IfSame);
}
void Recipe::add_library_path(const std::filesystem::path &path)
{
    object::Path p = object::Path(object::Type::LibraryPath, path);
    paths_.insert(p, cook_Overwrite_Always);
}



bool Recipe::merge(const Recipe &src)
{
    MSS_BEGIN(bool);

    src.paths_.each(object::Type::Library, [&](const object::Path & p)
    {
        if (p.propagation == cook_Propagation_Public)
        {
            if (p.owner == Owner::Me)
            {
                object::Path dep_p = p;
                dep_p.owner = Owner::Deps;
                paths_.insert(object::Type::Library, dep_p, cook_Overwrite_Always);
            }
            else
            {
                paths_.insert(object::Type::Library, p, cook_Overwrite_Always);
            }
        }
    });

    src.paths_.each(object::Type::LibraryPath, [&](const object::Path & p)
    {
        if (p.propagation == cook_Propagation_Public)
            paths_.insert(object::Type::LibraryPath, p, cook_Overwrite_Always);
    });

    src.paths_.each(object::Type::IncludePath, [&](const object::Path & p)
    {
        if (p.propagation == cook_Propagation_Public)
            paths_.insert(object::Type::IncludePath, p, cook_Overwrite_Always);
    });

    src.defines_.each(object::Type::Define, [&](const object::Define & p)
    {
        if (p.propagation == cook_Propagation_Public)
            defines_.insert(object::Type::Define, p, cook_Overwrite_IfSame);
    });



    if (src.type().empty())
    {
        src.files_.each({ object::Type::Header, object::Type::Source }, [&](object::Type type, object::File p)
        {
            p.owner = Owner::Deps;
            files_.insert(type, p, cook_Overwrite_Always);
        });
    }
    else if (src.type() == "library")
    {
        add_library(src.output().name, Propagation::Global, Owner::Me);
        add_library_path("./");
    }

    MSS_END();
}

toolchain::Libraries Recipe::libraries(Owner::Type owner) const
{
    toolchain::Libraries libs;

    paths_.each(object::Type::Library, [&](const object::Path & p)
    {
        if (p.owner & owner)
            libs.push_front(p.path);
    });

    return libs;
}
toolchain::Libraries Recipe::libraries(int owner) const
{
    return libraries((Owner::Type)owner);
}

toolchain::LibraryPaths Recipe::library_paths() const
{
    toolchain::LibraryPaths paths;
    paths_.each(object::Type::LibraryPath, [&](const object::Path & p) { paths.push_back(p.path); });

    return paths;
}

std::set<std::filesystem::path> Recipe::include_paths(Owner::Type owner) const
{       
    std::set<std::filesystem::path> ips_set;
    paths_.each(object::Type::IncludePath, [&](const object::Path & p) { ips_set.insert(p.path); });

    return ips_set;
}

toolchain::ForceIncludes Recipe::force_includes() const
{
    toolchain::ForceIncludes fis;
    files_.each(object::Type::ForceInclude, [&](const object::File & p) { fis.push_back(p.rel.string()); });

    return fis;
}

toolchain::Defines Recipe::defines() const
{
    toolchain::Defines defs;
    defines_.each(object::Type::Define, [&](const object::Define & p) { defs.push_back(std::make_pair(p.name, p.value)); });

    return defs;
}

bool Recipe::add_define(const std::string & macro, Overwrite overwrite)
{
    return add_define_(object::Define(macro), overwrite);
}

bool Recipe::add_define(const std::string & macro, const std::string & value, Overwrite overwrite)
{
    return add_define_(object::Define(macro, value), overwrite);
}

bool Recipe::add_define_(const object::Define & define, Overwrite overwrite)
{
    cook_Overwrite_t ov = cook_Overwrite_IfSame;
    switch(overwrite)
    {
    case Overwrite::Always: ov = cook_Overwrite_Always; break;
    case Overwrite::Never: ov = cook_Overwrite_Never; break;
    }
    return defines_.insert(define, ov);
}

void Recipe::add_include_path(const std::string & path, Propagation propagation)
{
    std::filesystem::path dir = path.empty() ? "." : path;
    if (dir.is_relative())
        dir = wd_/dir;

    add_include_path_(dir.string(), propagation);
}



void Recipe::stream(std::ostream &os) const
{
    os << "Recipe " << name() << ", uri: " << uri_hr() << ", type: " << type_ << ", working directory: " << wd_ << ", nr deps: " << deps_.size() << std::endl;

    files_.each({object::Type::Source, object::Type::Header}, [&](object::Type t, const object::File & p) { os << C(p.dir)C(p.rel)C(p.file_type)C(p.language) << std::endl; });

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

void Recipe::add_include_path_(const std::string & path, Propagation propagation)
{
    // do we have this include path ?
    object::Path * p = paths_.get(object::Type::IncludePath, path);

    // find out the propagation
    cook_Propagation_t prop = cook_Propagation_Private;
    if (propagation == Propagation::Global)
        prop = cook_Propagation_Public;

    if (p)
    {
        p->propagation = std::max(p->propagation, prop);
    }
    else
    {
        object::Path p = object::Path(object::Type::IncludePath, path);
        p.owner = Owner::External;
        p.propagation = prop;
        paths_.insert(p, cook_Overwrite_Always);
    }
}

void Recipe::update_output_()
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
std::ostream & operator<<(std::ostream &os, const Recipe &r)
{
    r.stream(os);
    return os;
}

} }
