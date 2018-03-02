namespace :boost do
    
    module_dir = "modules"
    tag = "boost-1.66.0"
    
    modules = []
    
    task :parse do
        File.open('required.dependencies', 'r').each do |line|
            
            line = line.sub("\n", '')
            
            mod_name = line.sub("~", "_")
            modules << mod_name
        end
    end
    
    task :create_recipe => [:parse]do
        File.open('dependencies.chai', 'w') do |f|
            ar = modules.map { |k| "\"#{k}\"" }.join(", ")
            f << "auto boosts = [#{ar}]\n" 
        end
    end
    
    
    
    task :generate_modules => [:parse] do
        
        Dir.chdir(module_dir) do
            modules.each { |k| sh "git submodule add -f https://github.com/boostorg/#{k}.git" }
            sh "git submodule update --init --recursive"
            modules.each do |k| 
                Dir.chdir(k) { sh "git checkout #{tag}" }
            end
        end
    end
    
    task :clean_modules => [:parse] do
        Dir.chdir(module_dir) { sh "git rm -rf *" }
    end
end
