#ifndef HEADER_cook_Indent_hpp_ALREADY_INCLUDED
#define HEADER_cook_Indent_hpp_ALREADY_INCLUDED

#include <string>
#include <ostream>

namespace cook { 

    class Indent
    {
    public:
        Indent() = default;

        const std::string &str() const {return str_;}

        void change(int d)
        {
            level_ += d;
            str_.resize(std::max(level_, 0)*2, ' ');
        }

        class Scope
        {
        public:
            Scope(Indent &indent): indent_(&indent)
            {
                indent_->change(1);
            }
            Scope(Scope &&dying): indent_(dying.indent_)
            {
                dying.indent_ = nullptr;
            }
            ~Scope()
            {
                if (!indent_)
                    //dead
                    return;
                indent_->change(-1);
            }
        private:
            Indent *indent_ = nullptr;
        };
        Scope scope(){Scope scope(*this); return scope;}

    private:
        Indent(const Indent &);
        Indent &operator=(const Indent &);

        int level_ = 0;
        std::string str_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Indent &indent)
    {
        os << indent.str();
        return os;
    }

} 

#endif
