#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

#include "cook/model/object/Define.hpp"
#include "cook/model/object/Path.hpp"
#include "cook/model/object/File.hpp"
#include "cook/model/object/Collection.hpp"
#include "cook/model/Uri.hpp"
#include "cook/model/toolchain/Types.hpp"
#include "cook/model/Types.hpp"
#include "gubg/file/System.hpp"
#include "gubg/macro/capture.hpp"
#include "gubg/OnlyOnce.hpp"
#include "gubg/Range.hpp"
#include <set>
#include <map>
#include <ostream>

namespace cook { namespace model { 

class Book;



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

class Recipe
{
public:
    struct Output
    {
        std::filesystem::path filename;
        std::string name;
    };

    Recipe(const Uri &book_uri);

    void set_name(const std::string &name);
    std::string name() const;

    const std::string &type() const;

    const Uri &uri() const;
    std::string uri_hr(bool add_root = true) const;

    std::string display_name() const;

    const Output &output() const;

    bool set_type(const std::string &value);
    const std::filesystem::path & working_directory() const;
    void set_working_directory(const std::string &value);
    void set_dependency(const std::string &value);
    void set_display_name(const std::string &value);
    void add(const std::string & p_dir, const std::string & pattern, cook_Flags_t default_flags, const FileFilterPredicate & filter);



    bool merge(const Recipe &src);

    template <typename Ftor>
    bool each_file(Ftor ftor, Owner::Type owner = Owner::Anybody) const
    {
        MSS_BEGIN(bool);
        files_.each({object::Type::Header, object::Type::Source}, [&](object::Type t, const object::File & p) {
            if (p.owner & owner)
                MSS(ftor(p));
        });
        MSS_END();
    }

    toolchain::Libraries libraries(Owner::Type owner) const;
    toolchain::Libraries libraries(int owner) const;
    toolchain::LibraryPaths library_paths() const;

    template <typename Ftor> bool each_dependency(Ftor ftor)
    {
        MSS_BEGIN(bool);
        for (auto &p: deps_)
        {
            MSS(ftor(p.first, p.second));
        }
        MSS_END();
    }

    std::set<std::filesystem::path> include_paths(Owner::Type owner = Owner::Me) const;
    toolchain::ForceIncludes force_includes() const;

    using Defines = std::map<std::string, std::string>;
    toolchain::Defines defines() const;
    bool add_define(const std::string & macro, cook_Overwrite_t overwrite);
    bool add_define(const std::string & macro, const std::string & value, cook_Overwrite_t overwrite);

    void add_include_path(const std::string & path, cook_Propagation_t propagation= cook_Propagation_Private);
    void add_library(const std::string &lib, cook_Propagation_t propagation = cook_Propagation_Private, Owner::Type owner = Owner::External);
    void add_library_path(const std::filesystem::path &path, cook_Propagation_t propagation = cook_Propagation_Private);

    void stream(std::ostream &os) const;

private:
    void add_include_path_(const std::string & path, cook_Propagation_t propagation);
    bool add_define_(const object::Define & define, cook_Overwrite_t overwrite);
    void update_output_();

    Uri uri_;
    std::string display_name_;
    std::string type_;
    std::filesystem::path wd_;
    FilePerPath file_per_path_;
    std::map<std::string, Recipe *> deps_;
    Output output_;

    object::Collection<object::File> files_;
    object::Collection<object::Define> defines_;
    object::Collection<object::Path> paths_;


};

std::ostream &operator<<(std::ostream &os, const Recipe &r);

} } 

#endif
