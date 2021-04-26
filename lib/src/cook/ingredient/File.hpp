#ifndef HEADER_cook_ingredient_File_hpp_ALREADY_INCLUDED
#define HEADER_cook_ingredient_File_hpp_ALREADY_INCLUDED

#include "cook/ingredient/Base.hpp"
#include "cook/Log.hpp"
#include "gubg/std/filesystem.hpp"
#include <ostream>

namespace cook { namespace ingredient {

    class File : public Base<std::string>
    {
    public:
        File(const std::filesystem::path & dir, const std::filesystem::path & rel)
        : Base<std::string>((dir/rel).string()),
        dir_(dir),
        rel_(rel)
        {
        }
        
        File(const File & rhs, const std::filesystem::path & rdir)
        : File(gubg::filesystem::combine(rdir, rhs.dir()), rhs.rel())
        {
            this->set_propagation(rhs.propagation());
            this->set_overwrite(rhs.overwrite());
            this->set_owner(rhs.owner());
            this->set_content(rhs.content());
        }

        bool operator==(const File & rhs) const
        {
            return equal_(rhs)
            && dir() == rhs.dir()
            && rel() == rhs.rel();
        }

        Result merge(const File & rhs)
        {
            MSS_BEGIN(Result);

            auto ss = log::scope("merge ingredient");
            // {
            //     auto s = log::scope("source");
            //     rhs.stream();
            // }
            // {
            //     auto s = log::scope("target");
            //     stream();
            // }

            MSS(merge_(*this, rhs));

            dir_ = rhs.dir();
            rel_ = rhs.rel();

            MSS_END();
        }

        log::Scope stream(log::Importance importance = log::Importance()) const
        {
            const auto imp = log::importance(importance);

            return log::scope("file", imp, [&](auto &n) {
                    n.attr("full_name", key()).attr("dir", dir()).attr("rel", rel()).attr("prop", propagation());
                    });
        }

        const std::filesystem::path & dir() const { return dir_; }
        const std::filesystem::path & rel() const { return rel_; }

    private:
        std::filesystem::path dir_;
        std::filesystem::path rel_;
    };

    inline std::ostream &operator<<(std::ostream &os, const File &file)
    {
        os << "File" << file.dir() << " | " << file.rel();
        return os;
    }


} }

#endif
