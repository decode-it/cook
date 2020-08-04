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

## 1.1.11

* CMake link library order is topological for recipes
* Improved support custom gcclike toolchains via `-T archiver=<archiver>`
* CMake fix for comparing empty paths
* Added Support for Objective-C and Objective-C++ (in beta)

## 1.1.12

* Adding language standard also for Objective C/C++
* Fixed problem in std::filesystem::path normalization that could lead to wrong include paths
* Fixed issue with object paths not being placed in the temporary output directory

## 1.1.13

* fixed cmake generation

## 1.1.14

* [wip] Support for dynamic and static runtime for MSVC (MD, MT, MDd, MTd) and GCC/CLang via `-T md`, `-T mt`, `-T c++.runtime=<static|dynamic>`
** [todo] Add support for mixing debug/release and static/dynamic

## 1.2.0

* Boost liberation
* [wip] Support for dynamic and static runtime for MSVC (MD, MT, MDd, MTd) and GCC/CLang via `-T md`, `-T mt`, `-T c++.runtime=<static|dynamic>`
** [todo] Add support for mixing debug/release and static/dynamic

## 1.2.1

* Support for dynamic and static runtime for MSVC (MD, MT, MDd, MTd) and GCC/CLang via `-T md`, `-T mt`, `-T c++.runtime=<static|dynamic>`
* [wip] Support for .rc files for MSVC
* Escaping spaces for windows in ninja
* Added `has_config(key)` and `has_config(key, value)` for toolchain in ninja
* Added `remove_config(key)` and `remove_config(key, value)` for toolchain in ninja
* Wrap LIBPATH argument in double quotes.
* Naft and HTML show configuration

## 1.2.2

* Added support for cmake position-independent code
* Added support for multiple force include files for cmake via the `SHELL:` annotation (requires cmake 3.12+)
* CMake output handles files with spaces

## 1.2.3

* debug does not imply rtc by default (use -T rtc)
* globbing with no result yields a warning (use -v >= 2)

## 1.2.4

* Support for response files for input files (by default, gcc/clang/msvc link & archive use this)
* Update on ninja rule names

## 1.2.5

* Add quotes around all files in msvc, gcc and clang toolchain files.

## 1.2.6

* Quotes are no longer in toolchain files, but inside commands.

## 1.2.7

* `cmake_minimum_required` is now version 3.12 (from 3.1), which is needed for the `SHELL:` prefix in `target_compile_options()`
* Don't use /MD(d) option when compiler is msvc rc (Resource)
* Reverse the order of object files in the response file
* CMake: Archive from objects only possible

## 1.2.8

* Chaiscript File.open() works in binary mode now

## 1.2.9

* MSVC fix: added the `/DEBUG` flag during linking to allow actual debugging in debug mode
* When a compile toolchain element is missing, cook will now report what source files are actually requiring this language support.
* Added gcc support for sysroot
* Added gcc support for a53
* Fixed bug in gcc toolchain: enabling profiling via `-T profile` resulted in `-g` compilation flag iso `-pg`.

## 1.2.10
* Toolchain has a `set_intermediary_namer` function to set the name for intermediary files (temporary fix)
* chai file IO is by default non-binary

## 1.2.11
* File functionality exposed in chai (join, each\_line, absolute)

## 1.2.12 
* More file functionality exposed in chai (basename, dirname, extname)
* Exposing toolchain configuration per recipe in chai

## 1.2.13 
* Different extensions for assembly
* Response file for command line arguments

## 1.2.14 
* New TargetType Plugin for runtime-loaded libraries (mac bundle)

## 1.2.15
* Added `sysroot` support for gcc/clang linker as well
* Added support for Recipe.remove(dir, rel, flags, functor)
* Added documentation for Recip.remove()

## 1.2.16
* Type and Flag can easilty be extended, e.g., Type.A = Type.UserDefined()
* ToolchainElement has a `set_ingredient_processor` callback function:
   * bool (ToolchainElement, Recipe, Ingredient): should return true if the propagation for this ingredient should stop, false otherwise
* `include` has no automatic guards against multiple inclusion
* Command line option `-I` adds extra directories to seach when using `include` in chai
* `include_relative` only tries to include from the relative directory
* `load` loads and evaluates the file without any guards

## 1.2.17
* Better propagation of linked libraries

## 1.2.18
* Archives are deleted before rebuilding

## 1.2.19
* Quoting arguments if command should delete before rebuiding (on windows)

## 1.2.20
* Support for older LIBC version for linux (built on top of ubuntu16.04)

## Next

* Propagating all `key_values` and `files` to toolchain elements
* Support for `COOK_PATH` search for scripts
* Internal #include-based dependency detection for compilers that cannot output dependencies
* Defines specified at toolchain level (eg NOMINMAX for MSVC) should be translated into cmake as well
* Add support for relative (parent) path in the recipe.add() method. e.g. `r.add("some/include/dir", "../some/other/path/*.[hp](pp)?")`
* Support in cmake for self-chosen archive name, see `scenario/app_arch_func`
* Link with gcc iso g++, adding -lstdc++ when binary items are present that carry the c++ language. When adding libraries, we should be able to control this, and avoid adding the -lstdc++ lib during linking.
