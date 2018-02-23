#ifndef HEADER_cook_util_ElementAt_hpp_ALREADY_INCLUDED
#define HEADER_cook_util_ElementAt_hpp_ALREADY_INCLUDED

#include <utility>

namespace cook { namespace util {

template <std::size_t I> struct ElementAt
{
    template <typename T> const typename std::tuple_element<I, T>::type & operator()(const T & t) const { return std::get<I>(t); }
    template <typename T> typename std::tuple_element<I, T>::type & operator()(T & t) const             { return std::get<I>(t); }
};

} }

#endif
