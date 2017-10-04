#include "cook/structure/Uri.hpp"
#include "gubg/Strange.hpp"
#include "gubg/mss.hpp"
#include <sstream>
#include <cassert>

namespace cook { namespace structure {

    namespace  { 
        template <typename Container>
        void split_and_append(Container &dst, const std::string &str)
        {
            gubg::Strange strange(str);
            for (std::string part; strange.pop_until(part, Tag::Separator); dst.push_back(part))
                ;
            if (!strange.empty())
                dst.push_back(strange.str());;
        }
    } 

    Uri::Uri(const std::string & complete_path)
    {
        split_and_append(data_, complete_path);
    }

    Uri::Uri(const util::Range<iterator> & lst)
    {
        data_.assign(RANGE(lst));
    }

    Uri operator+(const Uri & lhs, const Uri & rhs)
    {
        Uri res = lhs;
        res += rhs;
        return res;
    }

    Uri operator+(const Uri & lhs, const Tag & rhs)
    {
        if(rhs.empty())
            return lhs;

        Uri res = lhs;
        res += rhs;
        return res;
    }

    Uri & Uri::operator+=(const Uri & rhs)
    {
        if (&rhs != this)
        {
            std::vector<Tag> d = rhs.data_;
            data_.insert(data_.end(), RANGE(d));
        }
        else
            data_.insert(data_.end(), RANGE(rhs.tags()));

        return *this;
    }

    Uri & Uri::operator+=(const Tag & rhs)
    {
        split_and_append(data_, rhs.string());
        return *this;
    }


    bool Uri::operator==(const Uri & rhs) const
    {
        return std::equal(RANGE(tags()), RANGE(rhs.tags()));
    }

    bool Uri::operator!=(const Uri & rhs) const
    {
        return !operator==(rhs);
    }

    bool Uri::operator<(const Uri & rhs) const
    {
        return data_ < rhs.data_;
    }

    const Tag & Uri::back() const
    {
        return data_.back();
    }

    std::string Uri::string() const
    {
        std::ostringstream str;
        str << *this;
        return str.str();
    }

    std::ostream & operator<<(std::ostream & str, const Uri & ns)
    {
        for (auto it = ns.data_.begin(); it != ns.data_.end(); str << *it++)
            if(it != ns.data_.begin())
                str << Tag::Separator;

        return str;
    }

    namespace {

        template <class T>
        inline void hash_combine(std::size_t& seed, const T& v)
        {
            std::hash<T> hasher;
            seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }

    }

    std::size_t Uri::hash() const
    {
        std::size_t seed = 0;
        for(const auto & tag : tags())
            hash_combine(seed, tag);
        return seed;
    }

    std::filesystem::path to_path(const Uri & uri)
    {
        std::filesystem::path p;

        for(Tag t : uri.tags())
            p /= t.string();
        return p;
    }

} }
