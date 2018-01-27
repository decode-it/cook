#ifndef HEADER_cook_model_toolchain_Convert_hpp_ALREADY_INCLUDED
#define HEADER_cook_model_toolchain_Convert_hpp_ALREADY_INCLUDED

#include <ostream>
#include <gubg/string_algo/substitute.hpp>

namespace cook { namespace model { namespace toolchain {


    template <typename T>
    class Convert
    {
    public:
        Convert(const T & t, char pattern, char replace)
            : t_(t),
              pattern_(pattern),
              replace_(replace)
        {
        }

        friend std::ostream & operator<<(std::ostream & str, const Convert<T> & cv)
        {
            for (char c : cv.t_)
                str << (c == cv.pattern_ ? cv.replace_ : c);
            return str;
        }

    private:
        const T & t_;
        char pattern_;
        char replace_;
    };

    template <typename T> Convert<T> convert_uri(const T & element, char replace = '.', char pattern = '/')
    {
        return Convert<T>(element, pattern, replace);
    }


} } }

#endif
