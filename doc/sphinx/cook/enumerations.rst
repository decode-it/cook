Enumerations
------------

   Cook exposes the following enumeration in chaiscript, essential for constructing and configuring recipes.

Ingredient enumeration types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The following enumerations are used for ingredients and define their role within a recipe.

.. enumeration:: Propagation

   The propagation enumeration decides the ingredients propagation role. If recipe A is dependent on recipe B, then all the ingredients in B with Propagation.Public will propagate to A, and the ingredients in B with Propagation.Private will not propagate.

   * ``Propagation.Public``: do propagate
   * ``Propagation.Private``: do not propagate

.. enumeration:: Overwrite

   The overwrite enumeration decides the ingredients overwrite role, i.e., when merging two ingredients with the same key. Depending on the value, overwriting is allowed. Otherwise, an error is thrown. When trying to merge two ingredients, the stricted Overwrite value is used.

   * ``Overwrite.Always``: Ingredients can be mergen, and should not even be compared
   * ``Overwrite.IfSame``: Ingredients can be merged if they represent the same ingredient (with the same flags)
   * ``Overwrite.Never``: Ingredients should never be merged.

.. enumeration:: Type

   Type enumeration decides the type of the ingredient.

   * ``Type.Undefined``
   * ``Type.Source``: A source file
   * ``Type.Header``: A header file, results into include paths based on the dir
   * ``Type.Export``
   * ``Type.Object``: An object file
   * ``Type.ForceInclude``: A forced include file
   * ``Type.IncludePath``: An include path
   * ``Type.LibraryPath``: A library path (for the linker)
   * ``Type.Library``: A library (for the linker)
   * ``Type.FrameworkPath``: A framework path (for the linker, mac only)
   * ``Type.Framework``: A framework (for the linker, mac only)
   * ``Type.Dependency``
   * ``Type.Define``: A preprocessor define
   * ``Type.Executable``
   * ``Type.UserDefined(cntr)``: User defined type number `cntr`

.. enumeration:: Language

   The different type of language this ingredient belongs to

   * ``Language.Undefined``
   * ``Language.Binary``: A binary file
   * ``Language.C``: A C file
   * ``Language.CXX``: A C++ file
   * ``Language.ObjectiveC``: An Objective-C file
   * ``Language.ObjectiveCXX``: An Objective-C++ file
   * ``Language.ASM``: An assembly file
   * ``Language.Resource``: A resource file
   * ``Language.Script``: A script file
   * ``Language.Definition``
   * ``Language.UserDefined(cntr)``: User defined language number `cntr`
   
Flag class
^^^^^^^^^^
Flag unifies the following four types into one class: :enum:`Propagation`, :enum:`Overwrite`, :enum:`Type`, and :enum:`Language`. For each of those enumeration types, at most one value can be set. So, for example, we could create a flag representing ``Language.Binary & Type.Object`` (with no Propagation or Overwrite), but not a flag with ``Language.Binary & Type.Object & Type.Library``.

.. class:: Flag([flags])

   Creates a Flag object. If no parameter is given, then no flag value is set.

   :params flags: An optional flag
   :type flags: Another Flag object or a value from Propagation, Overwrite, Type or Language.


.. method:: Flag.operator bool() -> bool

   Converts the given flag to a boolean value

   :returns: ``true`` if at least one enumeration value is set, ``false`` otherwise.
   :rtype: bool

.. method:: Flag.to_string() -> String

   Convert the flag object to a human readable string representation

   :returns: A human readable version of the flag values.
   :rtype: String 

.. method:: Flags.operator&(flag) -> Flag

   Construct a new flag object with exactly those enumertion value which occur in this flag and in flag

   :params flag: The other flag
   :type flag: A Flag, or a value from Propagation, Overwrite, Type or Language.

.. method:: Flag.operator==(flag) -> bool

   Check whether the current flag is equal to another flag.

   :returns: ``true`` if equal, ``false`` otherwise.
   :rtype: bool

.. method:: Flag.operator!=(floag) -> bool

   Check whether the current flag differs from another flag.

   :returns: ``true`` if different, ``false`` otherwise.
   :rtype: bool
   

.. method:: Flag.set(flags_to_set)

   Explictly set the the flags. For each of the enumeration type, if ``flags_to_set`` has a value for that type, then that value will be set in the current object.

   .. code-block:: ruby
      
      Flags f1 = Propagation.Private & Language.Binary
      Flags f2 = Language.Source & Type.Library
      f1.set(f2)
      assert(f1 & Language.Source)
      assert(!(f1 & Language.Source))

TargetType
^^^^^^^^^^

.. enumeration:: TargetType

   The TargetType enumeration is used to define the :meth:`Recipe.type` of a recipe.

   
   * ``TargetType.Undefined``
   * ``TargetType.Object``
   * ``TargetType.Archive``: Make a static archive
   * ``TargetType.SharedLibrary``: Make a shared library
   * ``TargetType.Executable``: Make an executable
   * ``TargetType.Script``: Execute a script

   
Hook
^^^^

.. enumeration:: Hook

   The Hook enumeration is used to attach configuration callbacks to recipes, set :meth:`Recipe.set_config_callback`

   * ``Hook.Selected``: The hook for when a chef selects a recipe for making.
   * ``Hook.Pre``: The hook right before the chef will make this recipe.
   * ``Hook.Post``: The hook right after this recipe has been made by the chef.


OS
^^

.. enumeration:: OS

   The host operating system. See 
        
   * ``OS.Windows``: Windows 
   * ``OS.Linux``: Linux
   * ``OS.MacOS``: Mac

.. method:: OS.my() -> OS

   Get the host operating system. By chai syntax, a user can also call the function ``my(OS)``, which seems more intuitive.

   :returns: The host operating system
   :rtype: :enum:`OS`

