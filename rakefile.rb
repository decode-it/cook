here = File.dirname(__FILE__)
gubg_dir = File.join(here, "extern")
ENV["gubg"] = gubg_dir
$LOAD_PATH << File.join(gubg_dir, "gubg.build/src")

begin
    require_relative("extern/gubg.build/bootstrap.rb")
rescue LoadError
    puts("This seems to be a fresh clone: I will update all required submodules for you.")
    sh "git submodule update --init"
    sh "rake uth prepare"
    retry
end
require("gubg/shared")

require("mkmf")
ninja_exe = find_executable("ninja")
case GUBG::os
when :windows
    if !ninja_exe
        ninja_exe = "#{ENV["gubg"]}/bin/ninja"
        puts "ninja could not be found, using local version from #{ninja_exe}"
    end
    if !find_executable("cl")
        puts "Could not find the msvc compiler \"cl\", trying to load it myself"
        begin
            require("gubg/msvc")
            GUBG::MSVC::load_compiler(:x64)
            if !find_executable("cl")
                puts "WARING: Could not find the msvc compiler \"cl\":"
                puts "* [must] Install Microsoft Visual Studio 2017"
                puts "* [optional] Run \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2017\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat\" or other bat file to load it"
                # raise "stop"
            end
        rescue LoadError
            puts "WARNING: Could not load gubg/msvc, run \"rake uth prepare\""
        end
    end
end

task :default do
    sh "rake -T"
end

gubg_submods = %w[build std math io algo chaiscript].map{|e| "extern/gubg.#{e}"}
cook_submods = %w[releases doc/sphinx/sphinx-chai]
all_submods = gubg_submods + cook_submods

namespace :gubg do
    task :prepare do
        GUBG::each_submod(submods: gubg_submods) { |info| sh "rake prepare" }
    end
    task :clean do
        GUBG::each_submod(submods: gubg_submods) { |info| sh "rake clean" }
    end
    task :proper do
        GUBG::each_submod(submods: gubg_submods) { |info| sh "rake proper" }
    end
end

desc "Prepare the submods"
task :prepare do
    Rake::Task["gubg:prepare"].invoke
end

desc "Clean"
task :clean do
    rm(FileList.new("**/*.obj"))
    rm(FileList.new("*.exe"))
    rm_f(FileList.new("compile_commands.json"))
    Rake::Task["b0:clean"].invoke
    Rake::Task["b1:clean"].invoke
    Rake::Task["gubg:clean"].invoke
end

desc "Proper"
task :proper do
    Rake::Task["gubg:proper"].invoke
end

desc "Update all to head"
task :uth do
    GUBG::each_submod(submods: all_submods) do |info|
        sh "git checkout #{info[:branch]}"
        sh "git pull --rebase"
        sh "rake uth" if (info[:name][/^gubg\./] and `rake -AT`["rake uth"])
    end
end

task :update => :uth

$b0_build_dir   = "build/b0"
$b0_tmp         = ".b0"
$b0_ut          = File.join($b0_build_dir, "unit_tests.exe")
$b0_cook        = File.join($b0_build_dir, "cook.exe")

$b1_build_dir   = "build/b1"
$b1_tmp         = ".b1"
$b1_ut          = File.join($b1_build_dir, "unit_tests.exe")
$b1_cook        = File.join($b1_build_dir, "cook.exe")

#Bootstrap level 0: uses manually updated ninja scripts
namespace :b0 do


    def b0_ninja_fn()
        case GUBG::os
        when :linux then "#{$b0_build_dir}/linux/gcc.ninja"
        when :windows then "#{$b0_build_dir}/windows/msvc.ninja"
        when :macos then "#{$b0_build_dir}/macos/clang.ninja"
        end
    end

    desc "bootstrap-level0: generate the ninja scripts (depends on gubg.build)"
    task :generate do
        require("gubg/build/expand_templates")
        GUBG::Build::expand_templates("#{$b0_build_dir}/compile.ninja")
    end


    desc "bootstrap-level0: Build and run the unit tests"
    task :ut, [:filter] do |t,args|
        filter = args[:filter] || "ut"
        filter = filter.split(":").map{|e|"[#{e}]"}*""
        Rake.sh(ninja_exe, "-f", b0_ninja_fn, "-v", $b0_ut)
        sh($b0_ut, "-a", "-d", "yes", filter)
    end

    desc "bootstrap-level0: Build b0-cook.exe"
    task :build, [:ninja_opts] => :generate do |t,args|
        ninja_opts = (args[:ninja_opts] || "").split(" ")
        # ninja_opts = "-v -j 1"
        Rake.sh(ninja_exe, "-f", b0_ninja_fn, $b0_cook, *ninja_opts)
    end

    desc "bootstrap-level0: Install b0-cook.exe as cook"
    task :install => :build do
        case GUBG::os
        when :linux, :macos then sh("sudo cp #{$b0_cook} /usr/local/bin/cook")
        when :windows then cp($b0_cook, File.join(GUBG::shared("bin"), "cook.exe"))
        end
    end

    desc "bootstrap-level0: Clean"
    task :clean do
        sh("#{ninja_exe} -f #{b0_ninja_fn} -t clean")
        rm_rf($b0_tmp)
    end

    desc "bootstrap-level0: Update rtags"
    task :rtags do
        puts "Make sure you have \"rdm &\" running somewhere."
        sh "#{ninja_exe} -f #{b0_ninja_fn} -t compdb compile > compile_commands.json"
        sh("rc -J"){}
    end

    desc "bootstrap-level0: Update rtags"
    task :dia => [:rtags, :build, :ut]
end

def toolchain_options()
  toolchain_options = {"c++.std" => 17, "c++.runtime" => "static", "release" => nil}
  case GUBG::os
  when :macos
    # For now, we always build for Intel x86_64. This works on M1 as well, although a bit slower.
    # TODO: Add both x86_64 and arm64 builds
    toolchain_options["arch"] = "x64"
    toolchain_options["target"] = "x86_64-apple-macos10.15"
  when :windows
    toolchain_options["arch"] = "x86"
  end
  toolchain_options
end

def publish(cook_executable)
  ext = case GUBG::os
        when :macos, :linux then ext = ""
        when :windows then ext = ".exe"
        else raise "stop" end

  version = `#{cook_executable} -h`[/cook version (\d+\.\d+\.\d+) /, 1]
  version_dir = GUBG::mkdir("releases/#{version}")
  cp "changelog.md", version_dir
  arch = toolchain_options["arch"]
  dst_dir = GUBG::mkdir("#{version_dir}/#{GUBG::os}/#{arch}")
  cp cook_executable, File.join(dst_dir, "cook#{ext}")
  latest_dir = GUBG::mkdir("releases/latest/#{GUBG::os}/#{arch}")
  cp cook_executable, File.join(latest_dir, "cook#{ext}")
end

#Bootstrap level 1: uses output from bootstrap level 0
namespace :b1 do
    tmp_base = ".cook/b1"

    #    desc "bootstrap-level1: Build and run the unit tests"
    #    task :ut do
    #        sh("#{ninja_exe} -f #{b0_ninja_fn} -v unit_tests.exe")
    #        sh("./unit_tests.exe -a -d yes")
    #    end


    desc "bootstrap-level1: Build b1-cook.exe using b0-cook.exe"
    task :build, [:ninja_opts] do |t,args|

        ninja_opts = (args[:ninja_opts]||"").split(" ")
        Rake::Task["b0:build"].invoke(ninja_opts.join(" "))

        odir = File.join($b1_build_dir, "ninja")
        tdir = File.join($b1_tmp, "ninja")

        opts = toolchain_options.map {|k,v| ["-T"] + (v ? ["#{k}=#{v}"] : ["#{k}"]) }.flatten

        Rake.sh($b0_cook, "-f", "./", "-g", "ninja", "-o", odir, "-O", tdir, *opts, "cook/app/exe")
        Rake.sh(ninja_exe, "-f", "#{odir}/build.ninja", *ninja_opts)
        exe = "cook.app.exe"
        exe += ".exe" if GUBG::os == :windows
        cp "#{odir}/#{exe}", $b1_cook
    end

    desc "bootstrap-level1: Publish b1-cook.exe into releases"
    task :publish => :build do
      publish($b1_cook)
    end

    desc "bootstrap-level1-cmake: Build b1-cook.exe using b0-cook.exe"
    task :build_cmake => "b0:build" do
        cmake_dir = File.join($b1_tmp, "cmake")
        FileUtils.mkdir_p(cmake_dir) 
        current = Dir.pwd()
        sh "#{$b0_cook} -f ./ -g CMake cook/app/exe"
        Dir::chdir(cmake_dir) do
            sh "cmake #{current}"
            sh "make -j8"
        end
        cp "#{cmake_dir}/app/cook_app_exe", $b1_cook


    end

    desc "bootstrap-level1: Clean"
    task :clean do
        rm_rf $b1_build_dir
        rm_rf $b1_tmp
    end
end

desc "Build cook.exe"
task :build => "b1:build" do
    cp $b1_cook, "cook.exe"
end

def release_filename()
    case GUBG::os
    when :linux, :macos then "cook"
    when :windows then "cook.exe"
    end
end

desc "Install"
task :install, [:path] => "build" do |task, args|
    path = args[:path]
    if path
        GUBG::mkdir(path)
        sh("cp cook.exe #{path}")
    else
        case GUBG::os
        when :linux, :macos then sh("sudo cp cook.exe /usr/local/bin/cook")
        when :windows then cp("cook.exe", GUBG::shared("bin"))
        end
    end
end

namespace :docker do

  desc "Publish a docker build (with old GLIBC version)"
  task :publish do
    exe = case GUBG::os
          when :linux
            Dir.chdir("publish/linux") do
              # create the up-to-date docker
              Rake.sh("docker build -t cook/linux:latest ./")
              # build inside docker
              Rake.sh("docker run --rm -v #{Dir.pwd}:/var/publish cook/linux bash script.sh")

              File.join(Dir.pwd, "cook.exe")
            end
          else "No docker build available for #{GUGB::os}"
          end

    # publish the build artefact
    publish(exe)
    FileUtils.rm_rf(exe)
  end
end

namespace :doc do

    sphinxdir = "doc/sphinx"

    mdbook_exe = nil
    task :setup do
        sphinx_exe = File.join("/", "usr", "bin", "sphinx-build")
        puts sphinx_exe
        if !File.exists?(sphinx_exe)
            sh "yaourt -S python-sphinx"
        end
    end

    desc "Clean documentation"
    task :clean do
        Dir.chdir(sphinxdir) { sh "make clean" }
    end

    desc "Build documentation"
    task :build => :setup do
        Dir.chdir(sphinxdir) { sh "make html" }
    end


    desc "Publish the static pages on the web"
    task :publish => [:clean, :build] do
        Dir.chdir(File.join(sphinxdir, "_build/html")) do
            sh "scp -r * web-gfannes@ssh.linuxsystems.be:fannes.com/cook"
        end
    end
end

desc "Test"
task :test, [:filter] => "build" do |t,args|
    FileList.new("scenario/*.rb").each{|fn|require_relative(fn)}
    GUBG::Catch::run(args[:filter])
end
