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

## Next

* Support for `COOK_PATH` search for scripts
* Chaiscript-based toolchain
* Internal #include-based dependency detection for compilers that cannot output dependencies
* Arduino support: compilation, linking and programming
