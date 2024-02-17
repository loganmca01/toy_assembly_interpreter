# toy_assembly_interpreter
Customizable interpreter for toy assembly languages for use in computer organization classes.

# How to use:

NOTE: Only isa parser is operational at the moment, running is just for testing that ASTs are built correctly

1. Make sure you have Flex and Bison installed, comes pre installed in most linux distros, otherwise use these:
    https://www.gnu.org/software/bison/
    https://github.com/westes/flex

2. Run make
3. run ./isa_interpreter [file - for sample use test2.txt] [command name - with test2 try call or ret]


# Register Transfer Notation:

I designed this RTN to be as readable as possible for humans, as it's designed for use in classes. That being said, there are
several restrictions on the syntax in this first version of the parser. The basic command structure looks like this:

define [command name] [arguments] {[action 1];\
                                   [action 2];\
                                   [action 3];}

The arguments are limited to dummy variables and registers of the form var[a-z] and reg[a-z]. This will eventually
be changed to take any user variable names, but for now it lets me pre-generate a symbol table for the dummy arguments which
makes locating them in the table, and assigning them values when commands are run much simpler.

The actions must be separated by a new line and are in the form:

[register/mem location] := [expression/register/mem location] [optional conditional]

memory is referenced with parentheses, so (rega) means the contents of the location of the value rega in the stack.

expressions can be any operation between registers, dummy variables, and actual numbers. 

conditionals are represented by a : followed by some comparison.

Look at the file test2.txt for an example of how this all fits together.



# What's next:

- add further error handling in instruction set parser
- clean up grammar surrounding memory
- begin work on user input lexer and parser, to be run from same main
- create virtual environment with an array of chars for the stack and variables/symbols for registers
- add in more customization surrounding default registers, stack location in memory array, flags, word size, etc.
- put it all together


