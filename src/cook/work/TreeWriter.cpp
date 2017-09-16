#include "cook/work/TreeWriter.hpp"

namespace cook { namespace work {
    
    bool TreeWriter::operator()(const TopologicalOrder & order)
    {         
        MSS_BEGIN(bool);
        
        auto n = util::make_tree_node(std::cout, "recipes");
        
        // write all the books
        for(auto * book : order.books)
            MSS(write_book_(*book, n));
        
        for(auto * recipe: order.recipes)
            MSS(write_recipe_(*recipe, n, order));
        
        MSS_END();
    }
    
    bool TreeWriter::write_book_(const structure::Book & book, util::TreeNode & node)
    {
        // find all recipes in the book
        MSS_BEGIN(bool);
        
        auto nn = node.open("book");
        
        // properties
        {
            nn.attr("uri", book.uri());
            nn.attr("script", book.script_filename().native());
            nn.attr("display_name", book.display_name());
        }
        
        {
            for (const auto & p : book.elements())
                nn.open(to_s(p.second->type())).attr("uri", p.second->uri());
        }
        
        MSS_END();
        
    }
    
    bool TreeWriter::write_recipe_(const structure::Recipe & recipe, util::TreeNode & node, const TopologicalOrder & order)
    {
        MSS_BEGIN(bool);
        auto nn = node.open("recipe");
                
        // properties
        {
            nn.attr("uri", recipe.uri());
            nn.attr("tag", recipe.tag());
            nn.attr("script", recipe.script_filename().native());
            nn.attr("display_name", recipe.display_name());
            nn.attr("type", recipe.target_type());
            nn.attr("build_target", recipe.output().filename.native());
        }
        
        
        
        // streaming of files
        {
            auto to_s = [](structure::file::Type type)
            {
                switch (type)
                {
                    case structure::file::Source: return "source"; break;
                    case structure::file::Header: return "header"; break;
                    case structure::file::ForceInclude: return "force_include"; break;
                    
                }
                return "";
                
            };
            
            auto stream_files = [&](const auto & file_info)
            {
                for (const auto &p: file_info)
                    nn.open("file").attr("type", to_s(p.second.type)).attr("path", p.first.native());
            };
            
            stream_files(recipe.sources());
            stream_files(recipe.headers());
        }
        
        // extra material, coming from the dependencies
        structure::TargetConfig cfg;
        cfg = recipe.input_config();
        
        {
            std::list<structure::Recipe *> suborder;
            subset_order(std::back_inserter(suborder), recipe.uri(), util::make_range(order.recipes));
            
            for(auto it = suborder.begin(); it != suborder.end(); ++it)
            {
                auto * dep = *it;
                structure::merge(cfg, dep->output());
            }
        }
        
        // streaming of include paths
        {
            for(const auto & f : cfg.include_paths)
                nn.open("include_path").attr("path", f.native());
        }
        
        // streaming of library paths
        {
            for(const auto & f : cfg.library_paths)
                nn.open("library_path").attr("path", f.native());
        }
        
        // streaming of libraries
        {
            for(const auto & f : cfg.libraries)
                nn.open("library").attr("name", f);
        }
        
        
        
        // streaming of dependencies
        {
            for (const auto & dep : recipe.required_recipes())
                nn.open("depends_on").attr("uri", dep->uri());
        }
        
        
        
        MSS_END();
    }
} }
