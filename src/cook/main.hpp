#ifndef HEADER_cook_main_hpp_ALREADY_INCLUDED
#define HEADER_cook_main_hpp_ALREADY_INCLUDED

#include "cook/work/DependencyResolver.hpp"
#include "cook/chai/Loader.hpp"
#include "cook/Codes.hpp"
#include "cook/Options.hpp"
#include "cook/work/TreeWriter.hpp"
#include "cook/work/NinjaWriter.hpp"
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
    bool process_test(const Options & options);
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
    
    bool process_existing(const Options & options)
    {
        MSS_BEGIN(bool);
        
        if (options.clean)
        {
            std::cout << "Cleaning " << build_dir << std::endl;
            std::filesystem::remove_all(build_dir);
        }
        
        chai::Loader loader(options);
        structure::Book root(std::filesystem::path(options.path) / "recipes.chai");
        MSS(loader.load(root));
        
        work::DependencyResolver resolver;
        MSS(resolver.resolve(root), std::cerr << "Error resolving the dependencies" << std::endl);
        
        
        if(options.print_recipes)
        {
            work::TreeWriter writer;
            MSS(writer(resolver.order()));
        }
        
        
        if (!options.uri.empty())
        {
            structure::Uri uri(options.uri);
            MSS(!uri.empty(), std::cerr << "Invalid uri: " << options.uri << std::endl);
            
            work::TopologicalOrder order;
            MSS(work::simplify_order(order, uri, resolver.order()), std::cerr << "Could not locate uri " << uri << std::endl);
                
            work::NinjaWriter writer;
            std::ofstream ofs("build.ninja");
            
            writer.options.build_dir = ".cook";
            writer.options.output_dir = "";
            writer.options.compiler = "g++ -std=c++17";
            writer.options.linker= "g++ -std=c++17";
            writer.options.archiver= "ar rcs";
            writer.options.config = options.config;
            writer.options.additional_defines = (writer.options.config=="release" ? "-DNDEBUG" : "");
            writer.options.arch = options.arch;
            
            MSS(writer(ofs, order, uri));
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
} 

#endif
