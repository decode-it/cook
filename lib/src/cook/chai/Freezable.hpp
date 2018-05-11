#ifndef HEADER_cook_chai_Freezable_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Freezable_hpp_ALREADY_INCLUDED

namespace cook { namespace chai {

    class Freezable
    {
    public:
        Freezable() : is_frozen_(false) {}
        
        void freeze() { is_frozen_ = true; }
        bool is_frozen() const { return is_frozen_; }
        void set_freeze_flag(bool flag) { is_frozen_ = flag; }

    private:
        bool is_frozen_;
    };

} }

#endif

