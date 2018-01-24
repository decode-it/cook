#ifndef HEADER_cook_model_Uri_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Uri_hpp_ALREADY_INCLUDED

#include "gubg/Strange.hpp"
#include <vector>
#include <string>
#include <ostream>
#include <sstream>

namespace cook { namespace model { 

    class Uri
    {
    public:
        using Path = std::vector<std::string>;

        Uri(){}
        Uri(const std::string &uri)
        {
            gubg::Strange strange(uri);
            absolute_ = strange.pop_if('/');
            std::string part;
            while (strange.pop_until(part, '/'))
                add_path_part(part);
            set_name(strange.str());
        }

        void clear() {*this = Uri{};}

        bool absolute() const {return absolute_;}

        void add_path_part(const std::string &part) {path_.push_back(part);}
        void set_name(const std::string &name) {name_ = name;}
        const std::string &name() const {return name_;}

        size_t path_size() const {return path_.size();}
        void resize_path(size_t size) {path_.resize(size);}

        void append(const Uri &rhs)
        {
            for (const auto &part: rhs.path_)
                path_.push_back(part);
            name_ = rhs.name_;
        }

        template <typename Ftor>
        void each_path_part(Ftor ftor) const
        {
            for (const auto &part: path_)
                ftor(part);
        }

        std::string last_path() const
        {
            if (path_.empty())
                return "";
            return path_.back();
        }

        void stream(std::ostream &os, char root, char path_sep) const
        {
            add_separator_(os, (absolute_ ? root : '\0'));
            for (unsigned int ix = 0; ix < path_.size(); ++ix)
            {
                os << path_[ix];
                add_separator_(os, path_sep);
            }
            if (!name_.empty())
                os << name_;
        }
        void stream(std::ostream &os) const {stream(os, '/', '/');}

        std::string str(char root, char path_sep) const
        {
            std::ostringstream oss;
            stream(oss, root, path_sep);
            return oss.str();
        }
        std::string str() const {return str('/','/');}

    private:
        static void add_separator_(std::ostream &os, char ch)
        {
            if (ch == '\0')
                return;
            os << ch;
        }

        bool absolute_ = true;
        Path path_;
        std::string name_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Uri &uri)
    {
        uri.stream(os);
        return os;
    }

} } 

#endif
