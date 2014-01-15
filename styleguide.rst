Code Style
==========
One of the benefits of coding in C is that it offers a great deal of leeway in how the coding is done.
Though many people define certain conventions as being "right", there really *is* no one correct way of doing things.
However, there is one relatively agreed-upon point.
Whatever style you should use, you should use it consistently.
Thus, in an effort to define standards for code submission (and just to keep me consistent in my coding style), what follows is a small style book on how I write C code.

Variable Declaration
--------------------
When declaring a variable, the name should be separate from the data type.
Furthermore, variable names should be representative of what they are (avoid cryptic names) and, should be camel-case.
Next, pointer dereferencing should be separated from both the data type and the name.
If declared with an initializer, the initializer should be separated from the equals sign, just as the name is.
Below is an example of a good variable declaration:

``char * variableName = "data";``

Variable (Re)Definition and Usage
---------------------------------
If a variable was declared with no initializer and it is now being defined, or it is being redefined (regardless of whether or not it had an initializer), its assigned value should still be separated from the equals sign.
However, pointer dereferencing and referencing both are attached to the name.
Having the pointer (de)reference attached to the name is the same for usage.
Below is an example of a good variable definition and a good variable usage:

``*variableName = "new data";``

``if ( *variableName )``

Function Declaration
--------------------
Similar to variable declarations, function names should be representative and camel-case.
Also similar, data types should be separated from the function name.
The declarative argument list should be separated from the function name, and each item in the list should be separated by a comma, and a space.
When creating functions with generic names, prefix their names with an '_'.
Below is an example of a good function declaration with an argument list:

``char * _genericFunction (int firstArgument, char * secondArgument);``

*Note that each argument is formatted as if it were a regular variable declaration*

Indentation and Bracing
-----------------------
When defining a function, all the same guidelines for function declaration apply.
However, rather than ending the line with a ';', the argument list's close parenthesis ends the line.
On the following line, the function's opening brace is placed at the same indent level as the function name, and then (without a line-break) follows the first line of the function indented.
A standard indentation shall be four digits (this means that a function's first line will be spaced three times from the opening brace).
The function's closing brace will be left to a newline at the same indentation level as the opening brace and function name.
Below is an example of a good function definition showing good indentation and bracing style::
   char * _genericFunction (int firstArgument, char * secondArgument)
   {   firstArgument = 1;
       secondArgument = "some data";
       return secondArgument;
   }

*Note how this bracing style is similar to Horstmann's compromise between "K&R" and "Allman."*
Also note that *all* indentations should be spaces (not tabs).
This bracing style can be easily achieved in vim using the following modesetting line which should be the last line of the source file.

``// vim:set ts=4 sw=4 et:``

Calling Functions
-----------------
When calling a function, the function's argument list should be attached to the name.
All other guidelines from the function declaration and definition sections apply.
Below is an example of a good function call:

``_genericFunction(7, "data");``

Conditions
----------
Conditional blocks should always be spaced from the function and the opening parentheses.
In a for loop, each piece of the conditional block should be spaced from one another.
Below are a few examples of good conditional block formatting::
   while ( n < 7 )

   for ( i = 0; i < 34; i++ )

   if ( a == b )

*Note that the opening brace of each of these (should they have multiple lines of conditional execution) will follow the Indentation and Bracing guidelines.*
An if...elseif...else block will look like the following::
   if ( a > 0 )
   {   _runFunction();
       exit(1);
   }
   elseif ( a == 0 )
   {   _someOtherFunction();
       exit(2);
   }
   else
   {   printf("panic!\n");
       exit(3);
   }

Where applicable, if there are more than three cases (if it would require more than a single elseif), use of the ``switch`` function is preferred.

Section Labeling
----------------
It is often desireable to label various sections of the source document with comments.
When doing so, one should pick a consistent style of headings and sub-headings.
I prefer to use the following style::
   /* Section */

   // Subsection //

   /+ Subsubsection +/

Some basic section headings to contemplate using are "Includes", "Stack Variables", "Prototypes", "Main Function", "Function Definitions", etc.

Comments
--------
All comment texts should be spaced from the comment opening.
Multi-line comments should be delineated with a preceding '*' for each new line.
After the final line of text in a multi-line comment, add another new-line with a comment closing.
Below is an example of good commenting::
   foo * bar = "something"; // a comment
   
   /* Here is an example
    * of a multi-line
    * comment
    */

Disclaimer
----------
Personally, I find the above rules to create source documents that are incredibly well-formatted and easy-to-read.
They reflect experience and conclusions made from working with other styles.
Code submitted to this repository should follow this style guide; if it does not, the commit may either be rejected, or it will be reformatted to conform before being merged.
