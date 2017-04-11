gubg_dir = File.join(Dir.pwd, ".gubg")
ENV["gubg"] = gubg_dir

begin
    require("./gubg.build/shared.rb")
rescue LoadError
    puts("This seems to be a fresh clone: I will update all required submodules for you.")
    sh "git submodule update --init"
    retry
end

task :default do
    sh "rake -T"
end

namespace :setup do
    desc "Setup for ubuntu"
    task :ubuntu do
        #We rely on ninja
        sh "sudo apt install ninja-build"
        #Fixes problems with #including bits/c++config.h
        sh "sudo apt install gcc-multilib g++-multilib"
        Rake::Task["gubg:define"].invoke
    end
end

namespace :gubg do
    def each_mod()
        mods = %w[build std io chaiscript].map{|e|"gubg.#{e}"}
        mods.each do |mod|
            Dir.chdir(mod) do
                yield
            end
        end
    end

    task :proper do
        rm_rf gubg_dir
    end
    desc "Updates all submodules the their respective heads"
    task :uth do
        each_mod do
            sh "git checkout master"
            sh "git pull --rebase"
        end
    end
    task :declare do
        each_mod do
            sh "rake declare"
        end
    end
    task :define => :declare do
        each_mod do
            sh "rake define"
        end
    end
    task :ut => :define do
        require("gubg/build/Executable")
        ut = Build::Executable.new("unit_tests")
        ut.add_sources(FileList.new("gubg.io/src/test/file/System.cpp"))
        ut.add_sources("#{gubg_dir}/source/catch_runner.cpp")
        ut.add_include_path("#{gubg_dir}/include")
        ut.add_library_path("#{gubg_dir}/lib")
        ut.add_library("gubg.std", "gubg.io", "stdc++fs")
        case :debug
        when :debug
            ut.add_option('g')
        else
            ut.add_define('NDEBUG')
        end
        options = %w[-a -d yes]
        ut.run(options)
    end
end

module Ninja
    def self.build
        Rake::sh("ninja") do |ok, res|
            unless ok
                puts("Something went wrong, please run the setup for your platform")
                puts(" * Ubuntu: rake setup:ubuntu")
                raise("stop")
            end
        end
    end
end

namespace :cook do
    exe = nil
    task :init => "gubg:define" do
        require("gubg/build/Executable")
        exe = Build::Executable.new("cook")
        exe.add_sources(FileList.new("src/**/*.[hc]pp"))
        exe.add_include_path("src")
        %w[std io].each do |e|
            dir = "gubg.#{e}/src"
            exe.add_sources(FileList.new(File.join(dir, "gubg/**/*.[hc]pp")))
            exe.add_include_path(dir)
        end
        exe.add_include_path("#{gubg_dir}/include")
        exe.add_library("dl", "stdc++fs")
        case :debug
        when :debug
            exe.add_option('g')
        else
            exe.add_define('NDEBUG')
        end
    end
    task :clean do
        rm_rf(".cache")
    end
    task :build_with_ruby => :init do
        exe.build
    end
    task :build do
        if !File.exist?("cook.exe")
            if File.exist?("build.ninja")
                Ninja::build
            else
                Rake::Task["cook:build_with_ruby"].invoke
            end
        end
        sh "./cook.exe cook#exe"
        Ninja::build
    end
    task :run do
        sh "./cook.exe cook#exe"
    end
end
desc "Builds cook"
task :build => "cook:build"
desc "Tests cook"
task :test => ["cook:build", "cook:run"]

desc "Clean"
task :clean => "cook:clean" do
    rm(FileList.new("**/*.obj"))
    rm(FileList.new("*.exe"))
    rm_rf(".cook")
end

desc "Install"
task :install => "build" do
    sh("sudo cp cook.exe /usr/local/bin/cook")
end
