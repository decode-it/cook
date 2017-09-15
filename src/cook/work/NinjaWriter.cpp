#include "cook/work/NinjaWriter.hpp"

namespace cook { namespace work { 
    
    namespace {
        
        std::string convert_target(const structure::Uri & uri)
        {
            std::ostringstream str;
            
            for(char c : uri.string())
                str << (c == ' ' || c== '#' ? '_' : c);
            
            return str.str();
        }
        
        std::string convert_to_hr(const std::string & name)
        {
            std::ostringstream str;
            
            for(char c : name)
                str << (c == ' ' || c== '#' ? '_' : c);
            
            return str.str();
        }
        
        
        
        std::filesystem::path target_path(const structure::Uri & uri)
        {
            std::filesystem::path p;
            for(const auto & t : uri.tags())
                p /= t.string();
            return p;
        }
    }
    
    
    bool NinjaWriter::operator()(std::ostream & ofs, const TopologicalOrder & order, const structure::Uri & default_uri)
    {
        MSS_BEGIN(bool);
        
        MSS(decide_targets_(order));
        MSS(write_header_(ofs));
        
        for(auto * recipe : order.recipes)
            MSS(write_recipe_(ofs, *recipe));
        
        // add the default rule
        {    
            auto it = targets_.find(default_uri);
            MSS(it != targets_.end());
            const NinjaTarget & tgt = it->second;
            ofs << "default " << tgt.location.string() << std::endl; 
        }
        
        MSS_END();
    }
    
    bool NinjaWriter::decide_targets_(const TopologicalOrder & order)
    {
        MSS_BEGIN(bool);
        
        for(auto * recipe : order.recipes)
        {
            NinjaTarget tgt;
            
            {
                std::string tgt_filename;
                MSS(recipe->get_target_filename(tgt_filename));
                tgt.location = options.output_dir / tgt_filename;
            }
            
            tgt.type = recipe->target_type();
            
            for (auto * dep : recipe->required_recipes())
            {
                auto it = targets_.find(dep->uri());
                MSS(it != targets_.end());
                
                const NinjaTarget & dep_tgt = it->second;
                
                switch(dep_tgt.type)
                {
                    case structure::TargetType::StaticLibrary:
                        tgt.extra_objects.insert(dep_tgt.location);               
                        break;
                        
                    case structure::TargetType::Executable:
                        MSS(false, std::cerr << "recipe " << recipe->uri() << " depends on " << dep->uri() << ", but this is an executable" << std::endl);
                        
                    default:
                        MSS(false, std::cerr << "Unknown target type for " << dep->uri() << std::endl);       
                }               
                
                tgt.extra_objects.insert(dep_tgt.extra_objects.begin(), dep_tgt.extra_objects.end());
                tgt.extra_libraries.insert(dep_tgt.extra_libraries.begin(), dep_tgt.extra_libraries.end());
                tgt.extra_library_paths.insert(dep_tgt.extra_library_paths.begin(), dep_tgt.extra_library_paths.end());
            }
            
            targets_[recipe->uri()] = tgt;
        }
        
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
    
    bool NinjaWriter::write_recipe_(std::ostream & ofs, const structure::Recipe & recipe)
    {
        MSS_BEGIN(bool);
        // the settings for this recipe
        std::filesystem::path obj_tgt = options.build_dir / target_path(recipe.uri()) / std::filesystem::path(options.arch) / std::filesystem::path(options.config);
        
        auto it = targets_.find(recipe.uri());
        MSS(it != targets_.end());
        NinjaTarget & tgt = it->second;
                
        ofs << "# Recipe " << recipe.uri().string() << std::endl
            << "#       target type: " << recipe.target_type() << std::endl
            << "#   temporary files: " << obj_tgt.string() << std::endl
            << "#            result: " << tgt.location.string() << std::endl
            << "#" << std::endl;
        
        
        // the defines
        {
            ofs << "defines = " << options.additional_defines;
            for(const auto & p: recipe.defines())
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
            for(const auto & i : recipe.include_paths())
                ofs << " -I " << i;
            ofs << std::endl;
        }
        
        // the library paths
        {
            ofs << "library_paths =";
            for (const auto & l: recipe.library_paths())
                ofs << " -L" << l;
            for (const auto & l: tgt.extra_library_paths)
                ofs << " -L" << l;
            ofs << std::endl;
        }
        
        // the libraries
        {
            ofs << "libraries =";
            for (const auto & l: recipe.libraries())
                ofs << " -l" << l;
            for (const auto & l: tgt.extra_libraries)
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
            
            ofs << "build " << tgt.location.string() << ":  " << command;
            for(const auto & obj : objects)
                ofs << " $" << std::endl << "    " << obj.string();
            for(const auto & obj : tgt.extra_objects)
                ofs << " $" << std::endl << "    " << obj.string();
            ofs << std::endl << std::endl;
        }
        
        ofs << std::endl 
            << "###################################################################################################" << std::endl
            << std::endl << std::endl;
        
        MSS_END();
    }


} }

