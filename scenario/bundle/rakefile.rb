require("fileutils")

def doin(dir)
	orig_dir = Dir.pwd
	FileUtils.rm_rf(dir)
	FileUtils.mkdir_p(dir)
	Dir.chdir(dir) do
		yield(orig_dir)
	end
end

task :default => [:cmake, :make, :xcode]

task :cmake do
	sh "cook -g cmake"
	sh "cat CMakeLists.txt"
end

task :make do
	doin("build") do
		sh "cmake .."
		sh "make"
	end
end

task :xcode do
	doin("xcode") do
		sh "cmake .. -G Xcode"
		sh "xcodebuild -project tmp.xcodeproj"
	end
end

task :create_icns do
	name = "my_icon"
	#This should be 1024 x 1024
	master_png = "#{name}.png"

	icon_dir = "#{name}.iconset"
	doin(icon_dir) do
		#Create the different downscales
		sh "sips -z 16 16     ../#{master_png} --out icon_16x16.png"
		sh "sips -z 32 32     ../#{master_png} --out icon_16x16@2x.png"
		sh "sips -z 32 32     ../#{master_png} --out icon_32x32.png"
		sh "sips -z 64 64     ../#{master_png} --out icon_32x32@2x.png"
		sh "sips -z 128 128   ../#{master_png} --out icon_128x128.png"
		sh "sips -z 256 256   ../#{master_png} --out icon_128x128@2x.png"
		sh "sips -z 256 256   ../#{master_png} --out icon_256x256.png"
		sh "sips -z 512 512   ../#{master_png} --out icon_256x256@2x.png"
		sh "sips -z 512 512   ../#{master_png} --out icon_512x512.png"
		#Copy the original
		cp "../#{master_png}", "icon_512x512@2x.png"
	end
	sh "iconutil -c icns #{icon_dir}"
end

task :plist do
	fn = "Info.plist"

	# Create empty plist file
	File.open(fn, "w") do |fo|
		fo.puts('<?xml version="1.0" encoding="UTF-8"?>')
		fo.puts('<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">')
		fo.puts('<plist version="1.0">')
		fo.puts('<dict/>')
		fo.puts('</plist>')
	end
	sh "cat #{fn}"

	add_string = ->(key, value){
		sh "plutil -insert #{key} -string #{value} #{fn}"
	}
	add_string.call("CFBundleExecutable", "MyApp")
	add_string.call("CFBundleIconFile", "my_icon.icns")
	add_string.call("CFBundleIdentifier", "com.fannes.Test")
	add_string.call("CFBundleName", "MyApp")
	add_string.call("CFBundleDisplayName", "Test")
	add_string.call("CFBundlePackageType", "APPL")
	# add_string.call("CFBundlePackageType", "BNDL")
	add_string.call("CFBundleShortVersionString", "1.2.3")
	add_string.call("CFBundleVersion", "1.2.3.4")
	sh "cat #{fn}"
end