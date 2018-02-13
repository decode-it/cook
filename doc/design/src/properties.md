# Properties

Properties are categorized based on their Type and Language. At the time of writing, the following types and languages are known in cook:

 * Type:
    * Source
    * Header
    * ForceInclude
    * IncludePath
    * LibraryPath
    * Library
    * Define
    * Executable
 
 * Language:
    * C
    * C++
    * asm

All properties have a similar base interface, which can be defined implicit or explicit:
 * type()
 * language()
 * order(): The index within all properties with same type and language
 * propagation(): public if this property should propagate by default the *post*-recipe, private otherwise.
 * key(): A unique value within all properties with the same type and language.
 * owner(): The recipe responsable for creating this property. This allows for keeping track of the originator when propagating properties.
 

## Key value properties
A key value property is a property with key() a string and an additional element value(), also a string. An example of such a property is a define in C/C++, where the key corresponds to the macro name, and , if set, value to the definition.
 
## File properties
A file property is a property with key() a file path and with additional elements 
 * dir(): a file path
 * rel(): a file path
The requirements are that dir() is a directory, and path() == dir() / rel(). 
