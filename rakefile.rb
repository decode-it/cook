begin
    require("./gubg.build/shared.rb")
rescue LoadError
    sh "git submodule update --init"
    retry
end

task :default do
    sh "rake -T"
end

namespace :cook do
    exe = nil
    task :init do
        require("gubg/build/Executable")
        exe = Build::Executable.new("cook")
        exe.add_sources(FileList.new("src/**/*.cpp"))
        exe.add_include_path("src")
        exe.add_include_path("gubg.std/src")
    end
    task :build => :init do
        exe.build
    end
    task :run => :init do
        exe.run
    end
end
desc "Builds cook"
task :build => "cook:build"
desc "Tests cook"
task :test => ["cook:build", "cook:run"]
