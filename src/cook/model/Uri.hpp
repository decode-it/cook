#ifndef HEADER_cook_model_Uri_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Uri_hpp_ALREADY_INCLUDED

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

        void clear() {*this = Uri{};}

        void add_path_part(const std::string &part) {path_.push_back(part);}
        void set_name(const std::string &name) {name_ = name;}

        std::string last_path() const
        {
            if (path_.empty())
                return "";
            return path_.back();
        }

        void stream(std::ostream &os, char root, char path_sep, char name_sep) const
        {
            for (unsigned int ix = 0; ix < path_.size(); ++ix)
            {
                add_separator_(os, (ix == 0 ? root : path_sep));
                os << path_[ix];
            }
            if (!name_.empty())
            {
                add_separator_(os, name_sep);
                os << name_;
            }
        }
        std::string str(char root, char path_sep, char name_sep) const
        {
            std::ostringstream oss;
            stream(oss, root, path_sep, name_sep);
            return oss.str();
        }

    private:
        static void add_separator_(std::ostream &os, char ch)
        {
            if (ch == '\0')
                return;
            os << ch;
        }
        Path path_;
        std::string name_;
    };

} } 

#endif
