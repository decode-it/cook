
namespace :boost do
    cur_dir = File.dirname(__FILE__)
    
    module_dir = File.join(cur_dir, "modules")
    tag = "boost-1.66.0"
    
    modules = []
    
    task :parse do
        dep_file = File.join(cur_dir, 'required.dependencies')
        $boost_modules = []
        
        File.open(dep_file, 'r').each do |line|
            
            line = line.sub("\n", '')
            
            mod_name = line.sub("~", "_")
            
            $boost_modules << mod_name
        end
    end
    
    task :create_recipe => [:parse] do
        File.open(File.join(cur_dir, 'dependencies.chai'), 'w') do |f|
            ar = $boost_modules.map { |k| "\"#{k}\"" }.join(", ")
            f << "auto boosts = [#{ar}]\n" 
        end
    end
    
    task :generate_modules => [:parse] do
        Dir.chdir(module_dir) do
            $boost_modules.each { |k| sh "git submodule add -f https://github.com/boostorg/#{k}.git" }
            sh "git submodule update --init --recursive"
            $boost_modules.each do |k| 
                Dir.chdir(k) { sh "git checkout #{tag}" }
            end
        end
    end
    
    task :clean_modules => [:parse] do
        Dir.chdir(module_dir) { sh "git rm -rf *" }
    end
end
