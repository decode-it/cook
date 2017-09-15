#ifndef HEADER_cook_util_Range_hpp_ALREADY_INCLUDED
#define HEADER_cook_util_Range_hpp_ALREADY_INCLUDED

#include <iterator>
#define RANGE(range) (std::begin(range)), (std::end(range))

namespace cook { namespace util {

    template <typename It> class Range
    {
    public:
        using value_type = typename std::iterator_traits<It>::value_type;
        using reference = typename std::iterator_traits<It>::reference;
        using iterator = It;
        using const_iterator = It;
        
        Range() : valid_(false) {}
        
        template <typename OtherIt> Range(const std::pair<OtherIt, OtherIt> & p) : begin_(p.first), end_(p.second), valid_(true) {}
        template <typename OtherIt> Range(OtherIt first, OtherIt last) : begin_(first), end_(last), valid_(true) {}
        template <typename OtherRange> Range(const OtherRange & range) : begin_(range.begin()), end_(range.end()), valid_(true) {}
        template <typename OtherRange> Range(OtherRange & range) : begin_(range.begin()), end_(range.end()), valid_(true) {}
        
        
        bool operator==(const Range<It> & rng) const
        {
            return valid_ == rng.valid_ && begin_ == rng.begin_ && end_ == rng.end_;
        }
        bool operator!=(const Range<It> & rng) const
        {
            return !operator=(rng);
        }
        
        template <typename OtherIt> void set(OtherIt first, OtherIt last)
        {
            valid_ = true;
            begin_ = first;
            end_ = last;
        }
        template <typename OtherIt> void set(const std::pair<OtherIt, OtherIt> & p) { set(p.first, p.second); }
        template <typename OtherRange> void set(const OtherRange & range) { set(range.begin(), range.end()); }
        template <typename OtherRange> void set(OtherRange & range) { set(range.begin(), range.end()); }
        
        bool drop_front()
        {
            if (!valid_ || begin_ == end_)
                return false;
            
            ++begin_;
            return true;
        }
        
        bool drop_back()
        {
            if (!valid_ || begin_ == end_)
                return false;
            
            --end_;
            return true;
        }
        
        bool valid() const
        {
            return valid_;
        }
        
        std::size_t size() const 
        { 
            if (!valid_)
                return 0;
            return std::distance(begin_, end_);
        }
        
        bool empty() const 
        {
            return (valid_ ? begin_ == end_ : true);
        }
        
        iterator begin() const { return begin_; }
        iterator end() const        { return end_; }
        reference front() const { return *begin_; }
        reference back() const { It it = end_; return *it; }
        
    private:
        It begin_;
        It end_;
        bool valid_;
    };

    template <typename OtherRange>
    Range<typename OtherRange::iterator> make_range(OtherRange & range) { return Range<typename OtherRange::iterator>(range); }
    template <typename OtherRange>
    Range<typename OtherRange::const_iterator> make_range(const OtherRange & range) { return Range<typename OtherRange::const_iterator>(range); }
    template <typename It>
    Range<It> make_range(const std::pair<It, It> & p) { return Range<It>(p); }
    template <typename It>
    Range<It> make_range(It first, It last) { return Range<It>(first, last); }
} }

#endif
