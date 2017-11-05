#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

namespace cook { namespace model { 

    class Book;

    class Recipe
    {
    public:
        const std::string &name() const {return name_;}
        const std::string &type() const {return type_;}

        bool set(const std::string &key, const std::string &value)
        {
            MSS_BEGIN(bool);
            if (false) { }
            else if (key == "type") { MSS(value.empty() || value == "executable"); type_ = value; }
            else if (key == "working_directory") { wd_ = value; }
            MSS_END();
        }

        void add(const std::string &dir, const std::string &pattern)
        {
        }

        void stream(std::ostream &os) const
        {
            os << name_ << ", type: " << type_ << ", working directory: " << wd_ << std::endl;
        }

    private:
        friend class Book;

        std::string name_;
        std::string type_;
        std::string wd_;
    };

} } 

#endif
