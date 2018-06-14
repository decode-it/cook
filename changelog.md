Cook changelog
==============

## 1.0.0

Proof-of-concept version.

## 1.0.1

* Support for generating ninja files from chaiscript
* Support for naft-formatted information for the cook-qtcreator-plugin
* Rework towards model-view-presenter framework
* Support for static libraries and executables
* DAG-based recipe dependencies with limited data exchange between recipes
* Rough MSVC support

## 1.1.0

* Extensive rework, cleanup, restructuring and kitchen nomenclature
* Split into library and app
* Recipe and build graphs
* Support for multiple chaiscript input files
* Support for ninja, naft and cmake output
* Support for user data for books and recipes
* Speed-up of file globbing
* Support for stack-based error messages
* Support for force includes
* Support for chaiscript regular expressions, file io and os

## 1.1.1

* Added `-C void` chef to speed-up generation of naft output when only the recipe names are required.
* Support for debug/release/rtc/profile and x86/x64/x32 builds via the toolchain option
* Support for gcc, clang and msvc toolchains
* Support for linux, windows and macos
* Support for recursive header dependency detection

## 1.1.2

* Added support for `Book::has_recipe()`
* Added preliminary support for `arch=armv7`
* Preparation work for exposing the toolchain to chaiscript

## 1.1.3

* Toolchain export and specification via chaiscript
* Default toolchains for gcc, clang and msvc

## 1.1.4

* Changed the way to communicate the c/c++ standard: `-T c++.std=17` will now enable `-std=c++17`

## 1.1.5

* Added support for TargetType when specifying an element
* Built-in toolchain files are now installed in `.config/cook/<version>/` or `AppData/Local/cook/<version>/` if they do not exist yet
* Added arm64 support
* Added shared library support
* Added support Language.Definition and Type.Export to handle .def files when creating a .dll
* Fix for msvc defines

## 1.1.6

* Dependencies and build target are listed in naft generator
* Ingredients created in a recipe always have that recipe has owner
* Naft output follows types and variables as chai
* Added chai functionality for `add_file` and `add_key_value`
* Added chai `getenv()` function
* Resolving may fail if no chef is present
* No more backslashes in generated cmakelists
* Added /FS for msvc to allow parallel builds
* DLL fixes: .def files now have public propagation, /DEF is only active when building shared libraries

## 1.1.7

* Added cmake support for creating shared libraries
* Added Pre/Post configuration callback for recipes
* Chai global variable cook inherits book
* Chai recipe and book are copy constructable
* Chai recipe `each_file` and `each_key_value`
* Chai recipe build target name is adjustable
* Expose build target filename functionality in toolchain
* Generator selection is case-InsensItive
* Double dependency error better output 

## 1.1.8

* CMake better output, and less strict (skip script)
* Naft output always has build target filename (if present)

## 1.1.9

* CMake force include are platform independent
* Added Framework and FrameworkDir
* temporary output files are under <output_dir> / <recipe> / <md5 of dir> / <rel>
* Comparing chai recipes and books
* [Current|Output|Temporary] dir are exposed in chai (read-only)
* Flags are clonable (copy constructable)
* Reworked paths, both absolute and relative are possible
* Renamed `working_directory()` into `script_local_directory()`
* [wip] Support for HTML output

## 1.1.10

* Added "scal" and "cal" chefs: "scal" runs the scripts as well ("cal" behaviour from 1.1.9), "cal" will not run the scripts
* HTML generator now created the recipe details as well
* Improved support custom gcclike toolchains via `-T compiler=<compiler>` and `-T linker=<linker>`
* Added recipe-selected callback `Hook.Selected` which is called before any processing on recipes selected to be built
* Updated documentation

## 1.1.11 (open)

* CMake link library order is topological for recipes
* Improved support custom gcclike toolchains via `-T archiver=<archiver>`
* CMake fix for comparing empty paths
* Added Support for Objective-C and Objective-C++ (in beta)

## 1.2.0 (open)

* Boost liberation

## Next

* Propagating all `key_values` and `files` to toolchain elements
* Removing boost dependencies
* Support for `COOK_PATH` search for scripts
* Internal #include-based dependency detection for compilers that cannot output dependencies
* Defines specified at toolchain level (eg NOMINMAX for MSVC) should be translated into cmake as well
* Support for large MSVC link commands: when linking a lot of object files, the link command can become larger than 32k, causing an "CreateProcess: incorrect parameter" failure within ninja. Better is to add all the object filenames to a file, and give that to the linker.
