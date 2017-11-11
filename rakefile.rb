gubg_dir = File.join(Dir.pwd, ".gubg")
ENV["gubg"] = gubg_dir

begin
    require_relative("gubg.build/shared.rb")
rescue LoadError
    puts("This seems to be a fresh clone: I will update all required submodules for you.")
    sh "git submodule update --init"
    retry
end

task :default do
    sh "rake -T"
end

desc "Prepare the submods"
task :prepare do
    GUBG::each_submod do |info|
        sh "rake prepare"
    end
end
task :run

namespace :setup do
    desc "Setup for ubuntu"
    task :ubuntu do
        #We rely on ninja
        sh "sudo apt install ninja-build"
        #Fixes problems with #including bits/c++config.h
        sh "sudo apt install gcc-multilib g++-multilib"
        Rake::Task["gubg:run"].invoke
    end
end

desc "Update submodules to head"
task :uth do
    GUBG::each_submod do |info|
        sh "git checkout #{info[:branch]}"
        sh "git pull --rebase"
    end
end

task :update => :uth

namespace :gubg do
    task :proper do
        rm_rf gubg_dir
    end
    task :prepare do
        GUBG::each_submod do |info|
            sh "rake prepare"
        end
    end
    desc "Build and install gubg"
    task :run => :prepare do
        GUBG::each_submod do |info|
            sh "rake run"
        end
    end
end

def ninja_fn()
    case GUBG::os
    when :linux then "gcc.ninja"
    when :windows then "msvc.ninja"
    end
end

namespace :bcook do
    desc "Build bcook.exe"
    task :build do
        sh("ninja -f #{ninja_fn} -v")
    end
    desc "Clean"
    task :clean do
        sh("ninja -f #{ninja_fn} -t clean")
    end
end

desc "Build cook.exe"
task :build => "bcook:build" do
    cp "bcook.exe", "cook.exe"
end

desc "Clean"
task :clean do
    rm(FileList.new("**/*.obj"))
    rm(FileList.new("*.exe"))
    rm_rf(".bcook.*")
    Rake::Task["bcook:clean"].invoke
end

desc "Install"
task :install, [:bin] => "build" do |task, args|
    bin = args[:bin]
    if bin
        GUBG::mkdir(bin)
        sh("cp cook.exe #{bin}")
    else
        sh("sudo cp cook.exe /usr/local/bin/cook")
    end
end

desc "Create the documentation"
task :doc do
    Dir.chdir("doc") do
        %w[abc complex].each do |name|
            sh "dot -T png -o #{name}.png #{name}.dot"
            sh "mimeopen #{name}.png"
        end
    end
end

desc "Test"
task :test do
    Rake::Task["bcook:build"].invoke
    case :structure
    when :ninja
        sh "./bcook.exe -f test/app /app.exe"
        sh "cat test.ninja"
        sh "ninja -f test.ninja"
    when :details
        sh "./bcook.exe -f test/app -g details.tree /app.exe"
    when :structure
        sh "./bcook.exe -f test/app -g structure.tree /app.exe"
    end
end
