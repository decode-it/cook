#ifndef HEADER_cook_structure_Loader_hpp_ALREADY_INCLUDED
#define HEADER_cook_structure_Loader_hpp_ALREADY_INCLUDED

#include "cook/Options.hpp"
#include "cook/structure/Description.hpp"
#include "cook/Codes.hpp"
#include "cook/chai/Engine.hpp"
#include "gubg/mss.hpp"
#include <map>
#include <iostream>

namespace cook { namespace structure { 

    using DescriptionPerAlias = std::map<Alias, Description>;

    class Loader
    {
        public:
            Loader(const Options &options);

            ReturnCode load(std::filesystem::path dir, const std::filesystem::path &fn);

            ReturnCode resolve();

            ReturnCode get(const Description *&description, const Alias &alias) const;

            void stream(std::ostream &os) const;

            //Methods exposed to chai
            bool create_new_recipe_3(const std::string &ns, const std::string &tag, std::function<void (Description &)> callback)
            {
                return create_new_recipe_(ns, tag, "", callback);
            }
            bool create_new_recipe_4(const std::string &ns, const std::string &tag, const std::string &extra, std::function<void (Description &)> callback)
            {
                return create_new_recipe_(ns, tag, extra, callback);
            }
            bool add_dir(const std::string &dir);
            bool log(const std::string &dir);

        private:
            bool create_new_recipe_(const std::string &ns, const std::string &tag, const std::string &extra, std::function<void (Description &)> callback);
            std::string indent_() const;

            const Options &options_;
            chai::Engine chai_engine_;
            DescriptionPerAlias descriptions_;
            std::vector<std::filesystem::path> path_stack_;
    };

} } 

#endif
