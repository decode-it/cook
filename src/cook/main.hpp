#ifndef HEADER_cook_main_hpp_ALREADY_INCLUDED
#define HEADER_cook_main_hpp_ALREADY_INCLUDED

#include "cook/Model.hpp"
#include "cook/View.hpp"
#include "cook/Presenter.hpp"

#include "cook/work/DependencyResolver.hpp"
#include "cook/chai/Loader.hpp"
#include "cook/Codes.hpp"
#include "cook/view/Options.hpp"
#include "cook/work/TreeWriter.hpp"
#include "cook/work/NinjaWriter.hpp"
#include "gubg/mss.hpp"
#include "gubg/file/System.hpp"
#include "gubg/std/filesystem.hpp"
#include <fstream>
#include <set>
#include <cstdlib>

namespace cook { 
   
    void write_help(const view::Options & options);
    bool process_new(const view::Options & options);
    bool process_test(const view::Options & options);
    bool process_existing(const view::Options & options);

    ReturnCode main(int argc, const char **argv)
    {
        MSS_BEGIN(ReturnCode);

        auto mvp = [&](){
            MSS_BEGIN(bool);
            Model model;
            View view;
            Presenter presenter(model, view);

            MSS(view.process_cli(argc, argv));
            MSS_END();
        };
        const bool ok = mvp();
        std::cout << "MVP went " << (ok ? "OK" : "KO") << std::endl << std::endl;
        MSS_RETURN_OK();

        view::Options options;
        MSS(options.parse(argc, argv));
        
        view::Options::Mode mode;
        MSS(options.get_mode(mode), write_help(options));
        
        switch(mode)
        {
            case view::Options::Help:
                write_help(options);
                break;
                
            case view::Options::New:
                MSS(process_new(options));
                break;
                
            case view::Options::Existing:
                MSS(process_existing(options));
                break;
        }
        
        MSS_END();
    }
    
    bool process_existing(const view::Options & options)
    {
        MSS_BEGIN(bool);

        if (options.clean)
        {
            std::cout << "Cleaning " << options.build_dir << std::endl;
            try
            {
#ifdef _MSC_VER
#else
                std::filesystem::remove_all(options.build_dir);
#endif
            }
            catch(...)
            {
            }
        }
        
        chai::Loader loader(options);
        structure::Book root_book(std::filesystem::path(options.input_fod));
        MSS(loader.load(root_book));
        
        work::DependencyResolver resolver;
        {
            structure::Config config;
            config.build_dir = std::filesystem::path(options.build_dir);;
            MSS(resolver.resolve(root_book, config), std::cerr << "Error resolving the dependencies" << std::endl);
        }
        

        if (false) {}
        else if (options.generate == "recipes.tree")
        {
            work::TreeWriter writer;
            MSS(writer.static_structure(root_book, resolver.ordered_recipes()));
        }
        else
        {
            structure::Uri uri(options.uri);
            //TODO: use last recipe as default when uri is not valid
            MSS(!uri.empty(), std::cerr << "Invalid uri: " << options.uri << std::endl);

            work::Recipes suborder;
            {
                const auto & recipes = resolver.ordered_recipes();    
                work::subset_order(std::back_inserter(suborder), uri, util::make_range(recipes), true);
                MSS(!suborder.empty(), std::cerr << "Could not locate uri " << uri << std::endl);
            }

            if (false) {}
            else if (options.generate == "details.tree")
            {
                work::TreeWriter writer;
                MSS(writer.details(suborder, uri, options.build_dir));
            }
            else if (options.generate == "build.ninja")
            {
                std::filesystem::path ninja_file = options.build_dir;
                ninja_file /= "build.ninja";

                {
                    work::NinjaWriter writer;                  
                    std::ofstream ofs(ninja_file.string());

                    writer.options.build_dir = options.build_dir;
                    writer.toolchain.set_config(options.config);
                    writer.toolchain.set_arch(options.arch);

                    MSS(writer(ofs, suborder, uri));
                }

                if (options.do_build)
                {
                    std::string command = "ninja -v -f " + ninja_file.string();
                    MSS(std::system(command.c_str()) == 0);
                }
            }
        }

        MSS_END();
    }

    void write_help(const view::Options & options)
    {
        std::cout << options.help_message << std::endl;
    }

    bool process_new(const view::Options & options)
    {
        MSS_BEGIN(bool);
        std::cout << "Creating project " << options.project_name << std::endl;

        std::filesystem::path dir = options.input_fod;
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
