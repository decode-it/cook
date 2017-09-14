#ifndef HEADER_cook_main_hpp_ALREADY_INCLUDED
#define HEADER_cook_main_hpp_ALREADY_INCLUDED

#include "cook/structure/Loader.hpp"
#include "cook/Codes.hpp"
#include "cook/Options.hpp"
#include "gubg/mss.hpp"
#include "gubg/file/System.hpp"
#include "gubg/std/filesystem.hpp"
#include <fstream>
#include <set>
#include <cstdlib>

namespace cook { 

    const std::filesystem::path build_dir = ".cook";
    
    void write_help(const Options & options);
    bool process_new(const Options & options);
    bool process_existing(const Options & options);

    ReturnCode main(int argc, const char **argv)
    {
        MSS_BEGIN(ReturnCode);

        Options options;
        MSS(options.parse(argc, argv));
        
        Options::Mode mode;
        MSS(options.get_mode(mode), write_help(options));
        
        switch(mode)
        {
            case Options::Help:
                write_help(options);
                break;
                
            case Options::New:
                MSS(process_new(options), write_help(options));
                break;
                
            case Options::Existing:
                MSS(process_existing(options), write_help(options));
                break;
        }
        
        MSS_END();
    }
    
    void write_help(const Options & options)
    {
        std::cout << options.help_message << std::endl;
    }
    
    bool process_new(const Options & options)
    {
        MSS_BEGIN(bool);
        std::cout << "Creating project " << options.project_name << std::endl;

        std::filesystem::path dir = options.path;
        dir /= options.project_name;
        MSS(!std::filesystem::is_directory(dir), std::cout << "Error: project folder " << dir << " already exists" << std::endl);
        MSS(std::filesystem::create_directory(dir));

        {
            auto fn = dir; fn /= "rakefile.rb";
            MSS(!std::filesystem::is_regular_file(fn), std::cout << "Error: " << fn << " already exists" << std::endl);
            std::ofstream fo(fn);
            fo << "task :run do\nsh 'g++ src/main.cpp -o main'\nsh './main'\nend";   
        }
        
        {
            auto fn = dir; fn /= "recipes.chai";
            MSS(!std::filesystem::is_regular_file(fn), std::cout << "Error: " << fn << " already exists" << std::endl);
            std::ofstream fo(fn);
            fo << "lol";
        }
        
        {
            dir /= "src";
            MSS(std::filesystem::create_directory(dir));
            auto fn = dir; fn /= "main.cpp";
            MSS(!std::filesystem::is_regular_file(fn), std::cout << "Error: " << fn << " already exists" << std::endl);
            std::ofstream fo(fn);
            fo << "#include <iostream>\n\nusing namespace std;\n\nint main()\n{\n\n\treturn 0;\n}";
        }
        
        MSS_END();
    }
    
    bool process_existing(const Options & options)
    {
        MSS_BEGIN(bool);
        
        if (options.clean)
        {
            std::cout << "Cleaning " << build_dir << std::endl;
            std::filesystem::remove_all(build_dir);
        }
        
        if (options.print_recipes)
        {
            structure::Loader loader(options);
            MSS(loader.load("", "recipes.chai"));
            loader.stream(std::cout);
        }
        
        
        if (!options.alias.empty())
        {
            structure::Loader loader(options);
            MSS(loader.load("", "recipes.chai"));

            MSS(loader.resolve());

            const structure::Alias alias(options.alias);

            const structure::Description *ptr;
            MSS(loader.get(ptr, alias), std::cerr << "[error]{Could not find recipe for " << alias << "}" << std::endl);
            const auto &description = *ptr;

            if (options.verbose >= 2)
                description.print();

            structure::IncludePaths include_paths;
            description.get_all_include_paths(include_paths);

            structure::Defines defines = description.defines();
            if (options.config == "release")
                defines["NDEBUG"] = "";

            {
                std::ofstream fo("build.ninja");

                fo << "compiler = g++ -std=c++17" << std::endl;
                fo << "linker = g++ -std=c++17" << std::endl;
                fo << "delete = rm -f" << std::endl;
                fo << std::endl;

                fo << "cflags =";
                if (false) {}
                else if (options.arch == "x32") fo << " -m32";
                else if (options.arch == "x64") fo << " -m64";
                else
                {
                    std::cerr << "Unknown arch " << options.arch << std::endl;
                    MSS(false);
                }
                if (options.config == "debug")
                    fo << " -g ";
                fo << std::endl;
                fo << "defines = ";
                for (const auto &p: defines)
                {
                    const auto &key = p.first;
                    const auto &value = p.second;
                    if (value.empty())
                        fo << " -D" << key;
                    else
                        fo << " -D" << key << "=" << value;
                }
                fo << std::endl;
                fo << "include_paths =";
                for (const auto &ip: include_paths)
                    fo << " -I " << ip;
                fo << std::endl;

                fo << "lflags =";
                if (false) {}
                else if (options.arch == "x32") fo << " -m32";
                else if (options.arch == "x64") fo << " -m64";
                else
                {
                    std::cerr << "Unknown arch " << options.arch << std::endl;
                    MSS(false);
                }
                fo << std::endl;
                fo << "library_paths =";
                for (const auto &path: description.library_paths())
                    fo << " -L" << path;
                fo << std::endl;
                fo << "libraries =";
                for (const auto &lib: description.libraries())
                    fo << " -l" << lib;
                fo << std::endl;

                fo << "rule compile" << std::endl;
                fo << "  command = $compiler -c -MD -MF $out.d $cflags $defines -o $out $in $include_paths" << std::endl;
                fo << "  depfile = $out.d" << std::endl;
                fo << "rule link" << std::endl;
                fo << "  command = $linker $lflags -o $out $in $library_paths $libraries" << std::endl;
                fo << std::endl;

                std::list<std::filesystem::path> objects;
                for (const auto &p: description.sources())
                {
                    const auto &source = p.first;
                    auto obj = build_dir; obj /= alias.str(); obj /= options.arch; obj /= options.config; obj /= source; obj += ".obj";
                    objects.push_back(obj);
                    fo << "build " << obj.string() << ": compile " << source.string() << std::endl;
                }

                const std::filesystem::path exe = alias.ns()+"."+alias.tag();
                fo << "build " << exe.string() << ": link";
                for (const auto &obj: objects)
                    fo << " $" << std::endl << "    " << obj.string();
                fo << std::endl;
                fo << std::endl;

                fo << "rule clean" << std::endl;
                fo << "  command = ninja -t clean" << std::endl;
                fo << "build clean: clean" << std::endl;

                fo << "default " << exe.string() << std::endl;
            }

            if (options.do_build)
                MSS(std::system("ninja -v") == 0);
        }

        MSS_END();
    }

} 

#endif
