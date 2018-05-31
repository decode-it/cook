# Toolchains

Once cook has created the book-and-recipe hierarchy, it creates a _build graph_. This _directed acyclic graph_ contains all the operations required to gradually convert the different input files and information into the target output:

* _Compilation_ of source files into object files
* _Archiving_ object files into static libraries
* _Linking_ object files and static libraries into shared libraries or executables

Internally, the is orchestrated by the _CAL_-chef, short for _compile_, _archive_ and _link_.

The _ninja_ and _cmake_ generators translate this _build graph_ into an actual build system, but the _build graph_ information is not enough: these generators need to know how the _conceptual_ compile, archive and link operations, together with all the file and key-value information should be translated into actual command-line commands. This translation is the responsibility of the _toolchain_.

## Toolchain elements

A toolchain consists of different elements, each element responsible for the creation of the command-line command corresponding with:

* The compilation command for C++ programming language
* The compilation command for C programming language
* The compilation command for the assembly programming language
* The archive command
* The link command for shared libraries
* The link command for executables

Cook knows a few predefined toolchains (gcc, clang and msvc), which can be specified via the `-t <toolchain>` CLI argument. By default, cook will use the following toolchain, depending on the operating system:

* Linux: `gcc`
* MacOS: `clang`
* Windows: `msvc`

Once such a predefined toolchain is used, cook will export the [chaiscipt](https://chaiscript.org) version of it into the folder:

* Linux: `$HOME/.config/cook/<version>/toolchain/<toolchain>.chai`
* MacOS: `$HOME/.config/cook/<version>/toolchain/<toolchain>.chai`
* Windows: `%HOMEDRIVE%%HOMEPATH%/AppData/Local/cook/<version>/toolchain/<toolchain>.chai`

If this toolchain file is already present, the existing version will be used. This allows the user to easily experiment with altering toolchain settings; the version number that is integrated into the toolchain location ensures that cook can make breaking toolchain changes between different versions without requiring the user to manually clean the local configuration folder.

Additional toolchains can be specified via the `-t my_toolchain` CLI argument. Those that are not predefined, are expected by cook to exist as `my_toolchain.chai`.

## ToolchainElement translators and data

Each toolchain element consists of _data_ and a prioritised list _translators_. For each _build graph_ operation, the corresponding toolchain element is looked-up, and the data is _copied_ for that specific operation: the toolchain element can be populated with default data, which is updated and expanded with the specific data for the specific _build graph_ operation. Eg, a macro definition that is specified at recipe-level via the `rcp.define()` function will be added into the data for the compilation operations. If a system-wide macro should be specified, this macro value could be added to the _data_ at toolchain-element level.

Internally, the _data_ consists of different types of information:

* `Part.Cli`: the CLI executable.
* `Part.Pre`: options that come right after the CLI executable.
* `Part.Deps`: meant for the MSVC `/ShowIncludes` option to extract header dependency information.
* `Part.Export`: data expressing export information, like the `/DEF:` option for MSVC.
* `Part.Output`: data describing the output(s) of the CLI command, eg., output object filename for a compilation command.
* `Part.Input`: data describing the input(s) fo the CLI command, eg., input object filenames for an archive command.
* `Part.DepFile`: data describing how the header dependency file can be created.
* `Part.Option`: options later in the CLI
* `Part.Define`: macro definitions for compilation commands
* `Part.IncludePath`: include paths for compilation commands
* `Part.ForceInclude`: files to be force-included
* `Part.Library`: libraries to link against
* `Part.LibraryPath`: library search paths
* `Part.Framework`: macos frameworks to link against
* `Part.FrameworkPath`: macos framework search paths

Once all the _data_ for an operation is collected from the toolchain element and recipe information, this data is translated into a command-line command via the _translators_.

### Data

The data 
