#ifndef HEADER_cook_chai_Engine_hpp_ALREADY_INCLUDED
#define HEADER_cook_chai_Engine_hpp_ALREADY_INCLUDED

#include "gubg/std/filesystem.hpp"
#include "chaiscript/chaiscript.hpp"

namespace cook { namespace chai {

    class Engine
    {
        public:
            using Raw = chaiscript::ChaiScript_Basic;

            Engine();

            Raw &raw() {return engine_;}

            void eval_file(const std::filesystem::path &);

        private:
            Raw engine_;
    };

} }

#endif
