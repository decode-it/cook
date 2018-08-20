Ingredient
----------

.. class:: Ingredient

   An ingredient is a key element when constructing recipes. There are two types of ingredients, file or key_value.

.. method:: Ingredient.flags() -> Flags &

   Access the :class:`flags <Flag>` for this ingredient.

   :returns: The set of flags for this ingredient. This can be mutable.
   :rtype: :class:`Flag` &

.. method:: Ingredient.has_owner() -> bool

   Check whether the ingredient has an owner. If so, then the recipe which owns this ingredient can be called with the :meth:`Ingredient.owner` method.

   :returns: ``true`` if the ingredient has an owner, ``false`` otherwise.
   :rtype: bool

.. method:: Ingredient.owner() -> Recipe

   Retrieve the :class:`recipe <Recipe>` that owns this ingredient.

   :returns: The recipe containing this ingredient
   :rtype: :class:`Recipe`

.. method:: Ingredient.key() -> String

   Get the key for this ingredient. If the ingredient is a file, then this value equals ``"${dir()}/${rel()}"``, if it is a key_value, then the result is the key.

   :returns: The unique key for this recipe.
   :rtype: String

.. method:: Ingredient.is_file() -> bool

   Check whether this ingredient is a file. If this ingredient is a file, then the methods :meth:`File.dir` and :meth:`File.rel` are defined.

   :returns: ``true`` if it is a :class:`File`, ``false`` otherwise.
   :rtype: bool


.. method:: Ingredient.is_key_value() -> bool

   Check whether this ingredient is a key_valuee. If this ingredient is a key_value, then the methods :meth:`KeyValue.value` and :meth:`KeyValue.has_value` are defined.

   :returns: ``true`` if it is a :class:`KeyValue`, ``false`` otherwise.

   :rtype: bool

File
^^^^

.. class:: File(Ingredient)

   File inherits :class:`Ingredient` and represents a file on disk.  The :meth:`File.dir` and the :meth:`File.rel` form the key and the actual file reference. Files are either absolute, or relative to the recipe's :meth:`working dir<Recipe.working_dir>`.

.. method:: File.dir() -> String

   Get the dir part for this File
   
   :returns: The dir part 
   :rtype: String
   
.. method:: File.rel() -> bool

   Get the rel part for this File
   
   :returns: The rel part 
   :rtype: String

KeyValue
^^^^^^^^

.. class:: KeyValue(Ingredient)

   KeyValue inherits :class:`Ingredient` and is key value pair, where the value is optional.
   
.. method:: KeyValue.has_value() -> bool
   
   Query whether this KeyValue has an actual value.

   :returns: ``true`` if it has a value, ``false`` otherwise
   :rtype: bool

.. method:: KeyValue.value() -> String

   Get the value 

   :returns: The value or the empty string
   :rtype: String

