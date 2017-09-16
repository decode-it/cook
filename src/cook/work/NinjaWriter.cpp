#include "cook/work/NinjaWriter.hpp"
#include "cook/work/TopologicalOrder.hpp"

namespace cook { namespace work { 
    
    bool NinjaWriter::operator()(std::ostream & ofs, const std::list<Recipe *> & recipes, const structure::Uri & default_uri)
    {
        MSS_BEGIN(bool);

        MSS(write_header_(ofs));
        
        for(auto * recipe : recipes)
            MSS(write_recipe_(ofs, *recipe, recipes));
        
        // add the default rule
        ofs << "default " << default_uri << std::endl; 
        ofs << std::endl << std::endl;
        
        MSS_END();
    }
    
    bool NinjaWriter::write_header_(std::ostream & ofs)
    {
        MSS_BEGIN(bool);
        
        ofs << "compiler= " << options.compiler << std::endl
            << "linker= " << options.linker << std::endl
            << "archiver= " << options.archiver << std::endl
            << "rule compile" << std::endl
            << "  command = $compiler -c -MD -MF $out.d $cflags $defines -o $out $in $include_paths" << std::endl
            << "  depfile = $out.d" << std::endl
            << "rule link" << std::endl
            << "  command = $linker $lflags -o $out $in $library_paths $libraries" << std::endl
            << "rule archive" << std::endl
            << "  command = $archiver $out $in" << std::endl;
            
        ofs << "rule clean" << std::endl
            << "  command = ninja -t clean" << std::endl
            << "build clean: clean" << std::endl;       
        
        auto write_arch_flag = [&](const std::string & arch, const std::string prefix)
        {
            ofs << prefix << " =";
            MSS_BEGIN(bool);
            if (false) {}
            else if (options.arch == "x32") ofs << " -m32";
            else if (options.arch == "x64") ofs << " -m64";
            else
            {
                std::cerr << "Unknown arch " << options.arch << std::endl;
                MSS(false);
            }
            ofs << std::endl;
            MSS_END();
        };
        
        MSS(write_arch_flag(options.arch, "lflags"));
        MSS(write_arch_flag(options.arch, "cflags"));
        
        ofs << std::endl << std::endl;
            
        MSS_END();
    }
    
    bool NinjaWriter::write_recipe_(std::ostream & ofs, const Recipe & recipe, const std::list<Recipe *> & order)
    {
        MSS_BEGIN(bool);
        
        const auto & info = recipe.output();
        
        // the settings for this recipe
        std::filesystem::path obj_tgt = options.build_dir / to_path(recipe.uri()) / std::filesystem::path(options.arch) / std::filesystem::path(options.config);
                       
        ofs << "# Recipe " << recipe.uri().string() << std::endl
            << "#       target type: " << recipe.target_type() << std::endl
            << "#   temporary files: " << obj_tgt.string() << std::endl
            << "#            result: " << info.filename << std::endl
            << "#" << std::endl;
            
        // extra material, coming from the dependencies
        structure::TargetConfig cfg;
        cfg = recipe.input_config();
        
        {
            std::list<structure::Recipe *> suborder;
            subset_order(std::back_inserter(suborder), recipe.uri(), util::make_range(order));
            
            for(auto it = suborder.rbegin(); it != suborder.rend(); ++it)
            {
                auto * dep = *it;
                structure::merge(cfg, dep->output());
            }
        }
        
        
        // the defines
        {
            ofs << "defines = " << options.additional_defines;
            for(const auto & p: cfg.defines)
            {
                ofs << "-D" << p.first;
                if(p.second.empty())
                    ofs << "=" << p.second;
            }
            ofs << std::endl;
        }
        
        // the include include_paths
        {
            ofs << "include_paths =";
            for(const auto & i : cfg.include_paths)
                ofs << " -I " << i;
            ofs << std::endl;
        }
        
        // the library paths
        {
            ofs << "library_paths =";
            for (const auto & l: cfg.library_paths)
                ofs << " -L" << l;
            ofs << std::endl;
        }
        
        // the libraries
        {
            ofs << "libraries =";
            for (const auto & l: cfg.libraries)
                ofs << " -l" << l;
            ofs << std::endl;
        }
        
        // the build files
        std::list<std::filesystem::path> objects;
        for (const auto &p: recipe.sources())
        {
            const auto &source = p.first;
            std::filesystem::path obj = obj_tgt / source; obj += ".obj";
            objects.push_back(obj);
            ofs << "build " << obj.string() << ": compile " << source.string() << std::endl;            
        }
        
       
        // the linking
        {
            std::string command;
            switch (recipe.target_type())
            {
                case structure::TargetType::Executable:     command = "link"; break;
                case structure::TargetType::StaticLibrary:  command = "archive"; break;
                default:
                    MSS(false, std::cerr << "Unknown target type for '" << recipe.uri() << "'" << std::endl);
            }
            
            ofs << "build " << info.filename.native() << ":  " << command;
            for(const auto & obj : objects)
                ofs << " $" << std::endl << "    " << obj.string();
            
            if(!recipe.required_recipes().empty())
            {
                
                ofs << " | ";
                for (auto * dep : recipe.required_recipes())
                    ofs << dep->output().filename.native() << " ";
            }
            
            ofs << std::endl << std::endl;
        }
        
        ofs << "build " << recipe.uri() << ": phony " << info.filename.native() << std::endl;
        
        ofs << std::endl 
            << "###################################################################################################" << std::endl
            << std::endl << std::endl;
        
        MSS_END();
    }
} }
