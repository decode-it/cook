# Books and recipes

Cook allows the user to structure and modularize the build system information via _books_ and _recipes_.

## Books

A _book_ within cook is the mechanism to create a hierachycal structure, similar to directories for a filesystem, or namespaces within C++.

A book can be opened and re-opened as follows:

* `cook[uri]` or `root.book(uri)`
   * Directly returns the book specified via its `uri`, its _unique resource identifier_. 
* `root.book(uri, callback)`
   * The `callback` will be called with the book passed as its argument.

## Recipes

A _recipe_ within cook collects all information needed to create a specific build target. This information consists of:

* Paths and filenames, representing:
   * Source files
   * Header files
   * Include paths
   * Force-include files
   * Libary paths
   * Framework paths
   * Script files
   * Created object files
   * Created library files
   * Created shared library files
   * Created executable files
* Key-values, representing:
   * Defines
   * Libraries
   * Frameworks
* Recipe dependencies

### Creating a recipe

A recipe is always created _within_ a book, as follows:

* `book.recipe(name, callback)`:
   * The `uri` of this recipe is created by adding `name` to the `uri` of `book`.
   * The `callback` will be called, passing the created recipe as argument.
   * The `TargetType` for this recipe will be `TargetType.Undefined`.
* `book.recipe(name, target_type, callback)`
   * The `uri` of this recipe is created by adding `name` to the `uri` of `book`.
   * The `callback` will be called, passing the created recipe as argument.
   * This function allows specification of the `TargetType`.
* `book.recipe(name)`:
   * The `uri` of this recipe is created by adding `name` to the `uri` of `book`.
   * A reference to the actual recipe is returned
   * The `TargetType` for this recipe will be `TargetType.Undefined`.
* `book.recipe(name, target_type)`
   * The `uri` of this recipe is created by adding `name` to the `uri` of `book`.
   * A reference to the actual recipe is returned
   * This function allows specification of the `TargetType`.

### Recipe TargetType

A recipe within cook always has a single and fixed `TargetType`. Currently, the following `TargetType`s exist:

* `TargetType.Undefined`: a loose collection of files, this is the default `TargetType` if none is specified.
* `TargetType.Archive`: a static library.
* `TargetType.SharedLibrary`: a shared library: a `.dll` for windows, a `.so` for linux/macos.
* `TargetType.Executable`: an executable binary.
* `TargetType.Script`: an external command/script that can eg. be used to generate source files on-the-fly. The command is executed by cook itself _before_ globbing operations for dependent recipes are executed. As such, source files created by this command will be taken into account. Note that this means that this command will not appear in any generated build system, like the ones created by the ninja or cmake generators.
* `TargetType.UserDefined`: TBC

### Adding/reading info to/from a recipe

The following functions can be used to interact with a recipe:

* `rcp.type()` and `rcp.set_type(target_type)`: getter and setter for the `TargetType`.
* `rcp.working_dir()`: the directory of the chaiscript containing this recipe.
* `rcp.add(dir, pattern, [flags], [callback])`: add files to the recipe:
   * Depending on the file type, `dir` will be used as _include path_
   * When `flags` is not specified, the extension is used to guess the file type and programming language
* `rcp.remove(dir, pattern, [callback])`: removes files from the recipe:
   * Allows removal of files that were added by a generic globbing pattern
* `rcp.library(name)`: adds a library dependency to the recipe
* `rcp.library_path(path)`: adds a library path to the recipe
* `rcp.include_path(path)`: adds an include path to the recipe. Preferrable, include paths are added automatically via the `rcp.add()` function.
* `rcp.run(filename)`: adds a command to run, only used for `TargetType.Script`-recipes.
* `rcp.uri()`: returns the `uri` of this recipe
* `rcp.data()`: returns the _user-defined data_ for this recipe
* `rcp.depends_on(uri)`: indicates that `rcp` depends on the the recipe indicated via `uri`. `uri` can be _absolute_ or _relative_, for the latter, cook will resolve it starting from the location of `rcp` in the book/recipe hierarchy. Note that cook allows you add dependencies on recipes that do not exist yet. It even allows you to specify dependencies on recipes that will never exist: only when the dependent recipes are actually needed by a generator, will cook fail on this missing dependency.
* `rcp.add_file(dir, rel, [flags])`: basic method to add a single file. This function does not perform globbing nor automatic file type inference.
* `rcp.add_key_value(key, [value], [flags])`: basic method to add a single key-value pair.
* `rcp.name()`: returns a _reference_ to the _name_ of the primary target created by this recipe, eg., for a static library, this is the name without the _lib_ prefix and _.a_ extension (in case of gcc-style library naming convention). Because this is a reference, you can also assign to it.
* `rcp.primary_target_filename()`: returns the _filename_ of the primary target created by this recipe, eg., for a static library, this is the full filename, including prefixes and extensions.
* `rcp.each_file(callback)`: will run `callback` for each _file_ present in this recipe.
* `rcp.each_key_value(callback)`: will run `callback` for each _key-value_ present in this recipe.
* `rcp.set_config_callback()`: TBC

### Dependencies and propagation

By adding dependencies between recipes, we are creating a _directed graph_. To ensure cook can find an recipe order that respects all these dependencies, this graph should be a [directed acyclic graph](https://en.wikipedia.org/wiki/Directed_acyclic_graphhttps://en.wikipedia.org/wiki/Directed_acyclic_graph). An ordering of the recipes that respects all dependencies is called a _topological ordering_.

Once cook determined a topological ordering, it will process the recipes according to this ordering. In addition to this, files and key-value information is exchanged as well if the `Propagation` flag is `Public`. Depending on the recipe `TargetType`, the `Language` and `Type` of the different data items, cook determines these `Propagation` settings automatically. When needed, these can be overridden.

## URI

Books and recipes are identified via a _unique resource identifier_. Within [chaiscript](http://chaiscript.com/), a _uri_ is expressed as a string, using the `/` symbol to separate the book and recipe names.
