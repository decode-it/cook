#ifndef HEADER_cook_model_Book_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_Book_hpp_ALREADY_INCLUDED

#include <map>

namespace cook { namespace model { 

    class Book
    {
    public:
        Book() {}
        Book(std::string name): name_(name) {}

        Book &goc(std::string name)
        {
            auto &book = book_per_name_[name];
            book.name_ = name;
            return book;
        }

    private:
        std::string name_;
        std::map<std::string, Book> book_per_name_;
    };

    class BookStack
    {
    public:
        BookStack()
        {
            current_.push_back(&root_);
        }

        void push(const std::string &name)
        {
            Book &book = current_.back()->goc(name);
            current_.push_back(&book);
        }
        void pop()
        {
            current_.pop_back();
        }

    private:
        Book root_{"ROOT"};
        std::vector<Book*> current_;
    };

} } 

#endif
