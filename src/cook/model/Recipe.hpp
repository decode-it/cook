#ifndef HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Recipe_hpp_ALREADY_INCLUDED

namespace cook { namespace model { 

    class Book;

    class Recipe
    {
    public:
        const std::string &name() const {return name_;}

        void add(const std::string &dir, const std::string &pattern)
        {
        }

    private:
        friend class Book;

        std::string name_;
    };

} } 

#endif
