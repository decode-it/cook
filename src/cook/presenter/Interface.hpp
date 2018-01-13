#ifndef HEADER_cook_presenter_Interface_hpp_ALREADY_INCLUDED
#define HEADER_cook_presenter_Interface_hpp_ALREADY_INCLUDED

#include "cook/Types.hpp"
#include <string>

namespace cook { namespace presenter { 

    class Interface
    {
    public:
        virtual ~Interface() {}

        virtual bool set(const std::string &key, const std::string &value) = 0;
        virtual bool set(const std::string &key, const Strings &value) = 0;
        virtual bool set(const std::string &key) = 0;

        virtual bool get(const std::string &key, std::string & result) const = 0;
        virtual bool get(const std::string &key, const Strings & args, std::string & result) const = 0;
    };

    class Reference
    {
    public:
        void inject(Interface *itf) {itf_ = itf;}

        bool set(const std::string &key, const std::string &value) { return !!itf_ && itf_->set(key, value); }
        bool set(const std::string &key, const Strings &value) { return !!itf_ && itf_->set(key, value); }
        bool set(const std::string &key) { return !!itf_ && itf_->set(key); }
        bool get(const std::string &key, std::string & result) const { return !!itf_ && itf_->get(key, result); }
        bool get(const std::string &key, const Strings & args, std::string & result) const { return !!itf_ && itf_->get(key, args, result); }

    private:
        Interface *itf_ = nullptr;
    };

} } 

#endif
