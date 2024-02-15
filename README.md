# toy_assembly_interpreter
Customizable interpreter for toy assembly languages for use in computer organization classes.

How to use:

So far only the instruction set parser is in a working state. To get an idea for what this will be capable of,
run make in an environment with flex and bison installed, then run ./isa_interpreter test2.txt call. This 
will print out the abstract syntax tree built by the call command in the test2.txt file. The parser takes a list of commands
in a custom register transfer notation, forms ASTs with them, creates a command struct to store all of the information about
the instruction, then stores these in a symbol table.

Register Transfer Notation:

I designed this RTN to be as readable as possible for humans, as it's designed for use in classes. That being said, there are
several restrictions on the syntax in this first version of the parser. The basic command structure looks like this:

define [command name] [arguments] {[action 1];
                                   [action 2];
                                   ... }

The arguments are limited to dummy variables and registers of the form var[a-z] and reg[a-z]. This will eventually
be changed to take any user variable names, but for now it lets me pre-generate a symbol table for the dummy arguments which
makes locating them in the table, and assigning them values when commands are run much simpler.

The actions must be separated by a new line and are in the form:

[register/mem location] := [expression/register/mem location] [optional conditional]

memory is referenced with parentheses, so (rega) means the contents of the location of the value rega in the stack.

expressions can be any operation between registers, dummy variables, and actual numbers. 

conditionals are represented by a : followed by some comparison.

Look at the file test2.txt for an example of how this all fits together.



What's next:

- add further error handling in instruction set parser
- clean up grammar surrounding memory
- begin work on user input lexer and parser, to be run from same main
- create virtual environment with an array of chars for the stack and variables/symbols for registers
- add in more customization surrounding default registers, stack location in memory array, flags, word size, etc.
- put it all together


