.. _recipe:

Recipe
======

.. class:: Recipe

.. method:: Recipe.operator==(recipe) -> bool

   Check whether this recipe is the same as recipe. This is essentially the same as checking whether their :meth:`uris <Recipe.uri>` are equal. No in depth check is performed.

   :param recipe: The recipe to compare to.
   :type recipe: :class:`Recipe`
   :returns: ``true`` if equal, ``false`` otherwise
   :rtype: bool

.. method:: Recipe.operator!=(recipe) -> bool
   
   Check whether this recipe is a different recipe as another recipe. This is essentially the same as checking whether their :meth:`uris <Recipe.uri>` are different. No in depth check is performed.

   :param recipe: The recipe to compare to.
   :type recipe: :class:`Recipe`
   :returns: ``true`` if different, ``false`` otherwise
   :rtype: bool

.. method:: Recipe.set_type(target_type)

   Set the recipe type. This decides which actions are performed:
   
   * ``TargetType.Undefined``: see TargetType.Object
   * ``TargetType.Object``: Compiles source files into objects
   * ``TargetType.Archive``: Compiles source files and constructs a static archive
   * ``TargetType.SharedLibrary``: Compile source files and link into a dynamic library. This is library is linked into the executable and loaded at runtime.
   * ``TargetType.Plugin``: Compile source files and link into a dynamic library. This is library is not linked into the executable, but should be loaded at runtime using, e.g., dlopen.
   * ``TargetType.Executable``: Compile source files and link into an executable
   * ``TargetType.Script``: Call a script (see :meth:`Recipe.run`)

   :param target_type: The new recipe type
   :type target_type: :enum:`TargetType`

.. method:: Recipe.type() -> TargetType

   Get the current recipe type. If not type has been set with :meth:`Recipe.set_type` or with :meth:`Book.recipe`, then ``TargetType.Undefined`` is assumed.

   :returns: The target type of the recipe
   :rtype: :enum:`TargetType`


.. method:: Recipe.working_dir() -> String

   Get the working directory. This is the same as the directory where the recipe was first defined, i.e., the :meth:`cook.script_local_dir <Cook.script_local_dir>` of that chai-file.

   The working directory is important for :class:`File` ingredients: relative files are considered relative to the working directory.

   :returns: The working directory
   :rtype: String

.. method:: Recipe.add(dir, rel[, flags])
.. method:: Recipe.add(dir, rel, flags[, filter_functor])

   Add all files under ``dir`` matching regular expresssion ``rel``. These files are passed to the resolved (`Resolver`_) and then the flags are decided. By specifying some flags, the caller can override these flags. For example, if we would like to indicate that the file ``src/system/defines.hpp`` should be force included, we could do

   .. code-block:: ruby

      var r = cook.recipe("lib")
      r.add("src", "system/defines.hpp", Type.ForceInclude)

   Finally, the user can also supply a filter functor to filter out which files are passed to the resolver, or change the flags for those files. A filter functor should have the signature ``filter_functor(f) -> bool`` where ``f`` is a :class:`File` .
   
   :param dir: The directory. If relative, it is taken relative to :meth:`Recipe.working_dir`. No globbing or regular expression can be used.
   :type dir: String
   :param rel: The relative part. All files under dir matching globbing expression ``rel``, are considered.
   :type rel: String
   :param flags: The flags to overwrite the default flags are set by the resolver.
   :type flags: :class:`Flag`
   :param filter_functor: A functor to filter which files are passed to the resolver.
   :type filter_functor: A unary predicate accepting a :class:`File`.


.. method:: Recipe.remove(dir, rel[, flags])
.. method:: Recipe.remove(dir, rel, flags[, filter_functor])

   Remove all files under ``dir`` matching regular expresssion ``rel`` that were added before via the ``Recipe.add()`` function. These files are passed to the resolved (`Resolver`_) and then the flags are decided. By specifying some flags, the caller can override these flags.

   The user can also supply a filter functor to filter out which files are passed to the resolver, or change the flags for those files. A filter functor should have the signature ``filter_functor(f) -> bool`` where ``f`` is a :class:`File` .
   
   :param dir: The directory. If relative, it is taken relative to :meth:`Recipe.working_dir`. No globbing or regular expression can be used.
   :type dir: String
   :param rel: The relative part. All files under dir matching globbing expression ``rel``, are considered for removal.
   :type rel: String
   :param flags: The flags to overwrite the default flags are set by the resolver.
   :type flags: :class:`Flag`
   :param filter_functor: A functor to filter which files are passed to the resolver.
   :type filter_functor: A unary predicate accepting a :class:`File`.


.. method:: Recipe.library(name)

   Add a library to which this recipe should be linked. This method is shorthand for ``add_key_value(name, Propagation.Public & Overwrite.IfSame & Type.Library & Language.Binary)``. Therefore, even if this recipe will nog link, e.g., the :class:`Recipe.type` is ``TargetType.Object``, the dependency on this library will propagate. 

   :param name: The library name
   :type name: String
   
.. method:: Recipe.library_path(path)

   Add a library path in which libraries can be found. This method is shorthand for ``add_file(path, "", Propagation.Public & Overwrite.IfSame & Type.LibraryPath & Language.Binary)``. Therefore, even if this recipe will nog link, e.g., the :class:`Recipe.type` is ``TargetType.Object``, the library path will propagate. 

   :param path: The library path
   :type path: String
   
.. method:: Recipe.framework(name)
   
.. method:: Recipe.framework_path(dir)
   
.. method:: Recipe.include_path(path)

   Add an include path for this recipe. This method is shorthand for ``add_file(path, "", `Propagation.Public & Overwrite.IfSame & Type.IncludePath & Language.Undefined)``.
   
   Note that if header files are added, then the :meth:`File.dir` part is also added as an include path, so for example, the following recipe will have include path to `"src"`
   
   .. code-block:: ruby

   	var r = cook.recipe("lib")
	   r.add("src", "system/defines.hpp")

   :param dir: The include directory
   :type dir: String
   
   .. method:: Recipe.define(key[, value])
   .. method:: Recipe.define(key, value[, flags])

   Add a define to this recipe. This is shorthand for :meth:`Recipe.add_key_value()` with the supplied key, optional value, and optional flags. If no flags are set, then the default ``Propagation.Public & Overwrite.IfSame & Type.Define & Language.Undefined`` is used.
   
.. method:: Recipe.run(command)
   
.. method:: Recipe.uri() -> Uri

   Get the absolute Uri for this recipe.

   :returns: The uri for this recipe
   :rtype: :class:`Uri`
   
.. method:: Recipe.data() -> Data
   
.. method:: Recipe.depends_on(uri[, filter_functor])

   Add a dependency for this recipe on another recipe. If ``uri`` is relative, the resolver will try to find a recipe as follows:
   
   1. let ``l`` be our current uri
   2. If ``l / uri`` is a recipe, then return that recipe
   3. If l is not empty

      * then remove the last component of ``l``
      * goto 2

   4. return null

   For example,
   
   .. code-block:: ruby

      var r = cook.recipe("cook/lib/ut")
      r.depends_on("core/test")

   will search the following uris for the first match:
   
   1. ``/cook/lib/ut/core/test``
   2. ``/cook/lib/core/test``
   3. ``/cook/core/test``
   4. ``/core/test``

   Note that a missing dependency is allowed, as long as the chef will not prepare that recipe.

   :param uri: The uri
   :type uri: :class:`Uri` or String 
   
.. method:: Recipe.add_file(dir, rel[, flags])

   Add a :class:`File` to the list of ingredients, with specified dir and rel. If no flags are set, then following are used: ``Propagation.Public & Overwrite.IfSame & Language.Undefined & Type.Undefined``

   :param dir: The dir part of the file
   :type dir: String
   :param rel: The rel part of the file
   :type rel: String
   :param flags: The flags
   :type flags: :class:`Flag`
   
.. method:: Recipe.add_key_value(key[, flags])
.. method:: Recipe.add_key_value(key, value[, flags])
   
   Add a :class:`KeyValue` to the list of ingredients, with specified key, optional value. If no flags are set, then following are used: ``Propagation.Public & Overwrite.IfSame & Language.Undefined & Type.Undefined``

   :param keyr: The kry
   :type dir: String
   :param value: The value
   :type rel: String
   :param flags: The flags
   :type flags: :class:`Flag`
   
.. method:: Recipe.set_config_callback(hook, callback_functor)
   
.. method:: Recipe.primary_target_filename() -> String

   Get the name of the primary target (if present). This primary target filename is dependent on the :meth:`Recipe.name`, the operating system and the :meth:`target type <Recipe.type>`. For example, on linux we would have

   .. code-block:: ruby

      var r = cook.recipe("cook/lib/func")
      assert_equal(r.primary_target_name(), "libcook.lib.func.a")
      r.name = "cook"
      assert_equal(r.primary_target_name(), "libcook.a")
   
.. method:: Recipe.each_file(functor)
   
.. method:: Recipe.each_key_value(functor)
   
.. attribute:: name

   Accesses the name of the recipe. By default this equals ``uri.to_string('.')``, but a different name can be set. This influences the :meth:`Recipe.primary_target_filename`

   .. code-block:: ruby

      var r = cook.recipe("cook/lib/func")
      assert_equal(r.name, "cook.lib.func")
      r.name = cook.func
      assert_equal(r.name, "cook.func")




