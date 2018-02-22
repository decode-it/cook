namespace :boost do
    
    modules = {}
    
    task :parse do
        File.open('required.dependencies', 'r').each do |line|
            
            line = line.sub("\n", '')
            
            mod_name = line.sub("~", "_")
            mod_loc = line.sub("~", "/")
            
            modules[mod_name] = mod_loc
        end
    end
    
    task :create_recipe => [:parse]do
        File.open('dependencies.chai', 'w') do |f|
            ar = modules.map { |k,v| "\"#{v}\"" }.join(", ")
            f << "auto boosts = [#{ar}]\n" 
        end
    end
    
    task :update do
    end
end
