#ifndef HEADER_cook_presenter_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_presenter_Interface_hpp_ALREADY_INCLUDED

#include <string>
#include <any>

namespace cook { namespace presenter { 

    class Interface
    {
    public:
        virtual ~Interface() {}

        virtual bool set(const std::string &key, const std::any &value) = 0;
    };

    class Reference
    {
    public:
        void inject(Interface *itf) {itf_ = itf;}

        bool set(const std::string &key, const std::any &value) { return !!itf_ && itf_->set(key, value); }

    private:
        Interface *itf_ = nullptr;
    };

} } 

#endif
