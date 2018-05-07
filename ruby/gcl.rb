puts ">> GCC wrapper for MSVC"
require("rake")
Rake::sh("cl "+ARGV*' ')
puts "<< GCC wrapper for MSVC"
