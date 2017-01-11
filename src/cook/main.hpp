#ifndef HEADER_cook_main_hpp_ALREADY_INCLUDED
#define HEADER_cook_main_hpp_ALREADY_INCLUDED

#include "cook/Codes.hpp"
#include "cook/recipe/Loader.hpp"
#include "gubg/mss.hpp"
#include <fstream>
#include <set>

namespace cook { 
    ReturnCode main(int argc, const char **argv)
    {
        MSS_BEGIN(ReturnCode);

        recipe::Loader loader;
        MSS(loader.load("recipes.chai"));

        MSS(argc == 2);
        const recipe::Alias alias(argv[1]);

        const recipe::Description *ptr;
        MSS(loader.get(ptr, alias));
        const auto &description = *ptr;

        description.print();

        std::set<std::filesystem::path> include_paths;
        for (const auto &p: description.sources())
            include_paths.insert(p.second.dir);
        for (const auto &p: description.headers())
            include_paths.insert(p.second.dir);

        {
            std::ofstream fo("makefile");
            std::list<std::filesystem::path> objects;
            for (const auto &p: description.sources())
            {
                const auto &source = p.first;
                auto obj = source; obj += ".obj";
                objects.push_back(obj);
                fo << obj.string() << ": " << source.string() << std::endl;
                fo << "\tg++ -std=c++17 -c -o " << obj << " " << source;
                for (const auto &ip: include_paths)
                    fo << " -I " << ip;
                fo << std::endl;
            }
            const std::filesystem::path exe = "cook.exe";
            fo << exe.string() << ":";
            for (const auto &obj: objects)
                fo << " " << obj.string();
            fo << std::endl;
            fo << "\tg++ -std=c++17 -o " << exe;
            for (const auto &obj: objects)
                fo << " " << obj;
            for (const auto &lib: description.libraries())
                fo << " -l" << lib;
            fo << std::endl;
        }

        MSS_END();
    }
} 

#endif
