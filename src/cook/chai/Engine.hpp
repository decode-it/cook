#ifndef HEADER_cook_chai_Engine_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Engine_hpp_ALREADY_INCLUDED

#include "cook/Codes.hpp"
#include "gubg/file/Name.hpp"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace chai { 

    class Engine
    {
        public:
            Engine();

            ReturnCode eval_file(const gubg::file::Name &);

        private:
            chaiscript::ChaiScript_Basic engine_;
    };

} } 

#endif
