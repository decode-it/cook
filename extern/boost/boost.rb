namespace :boost do
    cur_dir = File.dirname(__FILE__)
    module_dir = File.join(cur_dir, "modules")
    tag = "boost-1.66.0"

    desc "boost: Update the boost modules to the required git hashes"
    task :update do
        sh "git submodule update --init --recursive #{module_dir}"
    end
    
    desc "boost: Loads the boost module names into $boost_modules"
    task :load do
        dep_file = File.join(cur_dir, 'required.dependencies')

        # Keep this global, this is used in b0-compile.ninja
        $boost_modules = File.open(dep_file).map { |line| line.chomp.sub("~", "_") }
    end
    
    desc "boost: Create chai script with the boost modules in the \"boosts\" variable"
    task :create_recipe => [:load] do
        File.open(File.join(cur_dir, 'dependencies.chai'), 'w') do |f|
            ar = $boost_modules.map { |k| "\"#{k}\"" }.join(", ")
            f << "auto boosts = [#{ar}]\n" 
        end
    end
    
    desc "boost: Add all the boost modules to the git repo"
    task :generate_modules => [:load] do
        Dir.chdir(module_dir) do
            $boost_modules.each do |k| 
                sh "git submodule add -f https://github.com/boostorg/#{k}"
                Dir.chdir(k) { sh "git checkout #{tag}" }
            end
        end
    end
    
    desc "boost: Removes all the boost modules from the git repo"
    task :clean_modules do
        Dir.chdir(module_dir) { sh "git rm -rf *" }
    end
end
