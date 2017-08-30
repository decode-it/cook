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
    task :ut => :run do
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
    cook_bootstrap = "cook_bootstrap"
    cook_bootstrap_fn = "#{cook_bootstrap}.exe"
    task :init do
        require("gubg/build/Executable")
        exe = Build::Executable.new(cook_bootstrap)
        exe.add_sources(FileList.new("src/**/*.[hc]pp"))
        exe.add_include_path("src")
        %w[std io].each do |e|
            dir = "gubg.#{e}/src"
            exe.add_sources(FileList.new(File.join(dir, "gubg/**/*.[hc]pp")))
            exe.add_include_path(dir)
        end
        exe.add_include_path("gubg.chaiscript/extern/ChaiScript/include")
        exe.add_library("dl", "stdc++fs", "pthread")
        case :debug
        when :debug
            exe.add_option('g')
        else
            exe.add_define('NDEBUG')
        end
    end
    task :clean do
        rm_rf(".cache")
	rm_f "build.ninja"
    end
    task :build_with_ruby => :init do
        exe.build
    end
    task :build do
        if !File.exist?(cook_bootstrap_fn)
            if File.exist?("build.ninja")
                puts("Building #{cook_bootstrap_fn} with the current build.ninja")
                Ninja::build
            end
        end
        if !File.exist?(cook_bootstrap_fn)
            puts("Building #{cook_bootstrap_fn} with ruby")
            Rake::Task["cook:build_with_ruby"].invoke
        end
        raise("Could not create #{cook_bootstrap_fn}") unless File.exist?(cook_bootstrap_fn)

        puts("Bootstrapping cook")
        sh "./#{cook_bootstrap_fn} cook#exe"
        Ninja::build
    end
    task :run do
        sh "./#{cook_bootstrap_fn} cook#exe"
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
task :install, [:bin] => "build" do |task, args|
    bin = args[:bin]
    if bin
        sh("cp cook.exe #{bin}")
    else
        sh("sudo cp cook.exe /usr/local/bin/cook")
    end
end
