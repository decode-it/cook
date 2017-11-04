#include "cook/View.hpp"

namespace cook { 

    void View::process(int argc, const char **argv)
    {
        send("help.message", std::string{"I need somebody"});
    }

} 
