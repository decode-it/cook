namespace :boost do
    cur_dir = File.dirname(__FILE__)
    module_dir = File.join(cur_dir, "modules")
    tag = "boost-1.66.0"
    dep_file = File.join(cur_dir, 'required.dependencies')
    
    require 'set'
    
    def extract_boost_modules(dir, src_dir)
        Dir::chdir(dir) do
            mods = Set.new
            IO.popen("dist/bin/boostdep --track-sources --subset-for #{src_dir}").each do |line|
                m = /^([a-zA-Z_~]+):/.match(line) 
                mods.add(m[1]) if m
            end
            mods
        end
    end
    
    def extract_all_boost_modules(dir, root_dir)
        mods = Set.new()
        ["lib", "app"].each { |p| mods += extract_boost_modules(dir, File.join(root_dir, p)) }
        return mods
    end
    
    $boost_modules = nil
    
        
    desc "boost: Create chai script with the boost modules in the \"boosts\" variable"
    task :create_recipe => [:load] do
        File.open(File.join(cur_dir, 'dependencies.chai'), 'w') do |f|
            ar = $boost_modules.map { |k| "\"#{k}\"" }.join(", ")
            f << "auto boosts = [#{ar}]\n" 
        end
    end
    
        
    desc "boost: Recreate the boost dependency list"
    task :deps, [:path] do |t,args|
        if $boost_modules == nil
            $boost_modules = extract_all_boost_modules(args[:path], File.join(cur_dir, "..", "..")).to_a
            File.open(dep_file, 'w') do |os|
                $boost_modules.each { |m| os << "#{m}\n" }
            end
        end
        
        Rake::Task["boost:create_recipe"].invoke
    end
       
    desc "boost: Loads the boost module names into $boost_modules"
    task :load do       
        # Keep this global, this is used in b0-compile.ninja
        $boost_modules = File.open(dep_file).map { |line| line.chomp.sub("~", "_") }
    end

    desc "boost: Update the modules"
    task :update => [:load] do
        Dir.chdir(module_dir) do
            $boost_modules.each do |k| 
                puts ">> #{k}"
                if Dir["#{k}/*"].empty?
                    FileUtils.rm_rf(k)
                end
                if !File.exists?(k)
                    sh "git submodule add -f https://github.com/boostorg/#{k}"
                end
                
                Dir.chdir(k) do
                    sh "git fetch origin"
                    sh "git checkout #{tag}" 
                end
                puts "<< #{k}"
            end
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
