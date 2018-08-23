.. _uri:

***
Uri
***

A uri, short for `uniform resource identifier <https://en.wikipedia.org/wiki/Uniform_Resource_Identifier>`_, is used in cook to uniquely identify :class:`books <Book>` and :class:`recipes <Recipe>`. Similarly as to a filesystem path, a uri consists of components, separated by a forward slash. For example, the uri ``/cook/lib/ut`` represents

:: 

   /
    └── cook
        └── lib
            └── ut

A Uri is absolute if it contains a leading slash, and relative otherwise. :class:`Book`.

.. class:: Uri([uri = ""])

   Constructs a uri. If no parameter is passed, then an empty, relative uri is constructed.

   :param uri: The optional value for the uri.
   :type uri: Uri or String
   

       
Operators
---------

To ease the usage of the URI class, the following operators have been defined.

   .. method:: Uri.operator/(uri) -> Uri

   Create a new Uri by concatenating this uri with and a given uri. The given uri is always interpreted to be relative. 

   :param uri: The uri to concatenate after this uri
   :type uri: Uri or String
   :returns: A new Uri
   :rtype: Uri
   
.. method:: Uri.operator/=(uri) -> Uri &

   Concatenate a new uri to this uri. The given ``uri`` is always interpreted to be relative.

   :param uri: The uri to concatenate to this uri.
   :type uri: Uri or String
   :returns: A reference to this uri.
   :rtype: Uri &
   
.. method:: Uri.operator=(uri) -> Uri &

   Assign this uri with the given value.

   :param uri: The new value for this uri.
   :type uri: Uri or String
   :returns: A reference to this uri.
   :rtype: Uri &
   
.. method:: Uri.operator==(uri) -> bool

   Check whether  this uri is equal to a given uri.

   :param uri: The object to compare to.
   :type uri: Uri
   :returns: ``true`` if equal, ``false`` otherwise.
   :rtype: bool
   
.. method:: Uri.operator!=(uri) -> bool

   Check whether two uris are unequal.

   :param uri: The object to compare to.
   :type uri: Uri
   :returns: ``true`` if equal, ``false`` otherwise.
   :rtype: bool

 

Conversion methods
------------------
   
.. method:: Uri.as_absolute() -> Uri

   Creates a new uri based on this uri which is guaranteed to be absolute, i.e., with a leading ``/``.

   :returns: A new uri
   :rtype: Uri
   
.. method:: Uri.as_relative() -> Uri
    
   Creates a new uri based on this uri which is guaranteed to be relative, i.e., without a leading ``/``.

   :returns: A new uri
   :rtype: Uri

.. method:: to_string([separator = '/']) -> String

   Get a textual representation for the uri. If a separator is supplied, that separator is used, otherwise the default forward slash (``/``) is used.

   :param separator: The separator to use
   :type separator: char
   :returns: a string representing the uri
   :rtype: String
