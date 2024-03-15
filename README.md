# toy_assembly_interpreter
Customizable interpreter for toy assembly languages for use in computer organization classes.

# How to use:

1. Make sure you have Flex and Bison installed, comes pre installed in most linux distros, otherwise use these:
    https://www.gnu.org/software/bison/
    https://github.com/westes/flex

2. Run make
3. run ./isa_interpreter [filepath]
4. run assembly instructions specified in file from command line or from text file

# System Commands

.help                                      --- print information about system and available commands\
\
.clear_stack [start address] [end address] --- clear the contents of the stack between two addresses\
\
.clear_code [start address] [end address]  --- clear the contents of code memory between two addresses\
\
.clear_all                                 --- clear everything from stack and code memory\
\
.file [filepath]                           --- load commands from file into next available code address, only need file name if in same folder\
\
_print_stack [start address] [end address] --- print the contents of stack between two addresses\
\
_print_code [start address] [end address]  --- print the contents of code memory between two addresses\
\
_exit                                      --- exit current instructions, functions the same as jumping PC to after last instruction\
\
NOTE: underscore commands are loaded into code memory, dot commands are not and therefore cannot be run from a file.


# Register Transfer Notation:

Command structure:

define [command name] [arguments] {\
[action 1];\
[action 2];\
[action 3];\
}

- Arguments are limited to dummy variables and registers that start with "var" or "reg".

- Actions must be separated by a new line, and are in the form:\
[register/mem location] := [expression/register/mem location] [optional conditional]

- memory is referenced with parentheses, so (rega) means the value on the stack at the location stored in rega.

- expressions can be any operation between registers, dummy variables, and actual numbers. 

- conditionals are represented by a : followed by some comparison operation (<, >, ==, <=, >=).

- The file test_asm1.txt shows an example of a toy language written in this RTN

# Version 1.0 notes
- First full working version, somewhat limited user customization of environment but instruction sets can be loaded and commands can be run
- Not memory safe, next priority is removing any possible memory leaks

  
# What's next:

- Add in more customization surrounding built-in registers, stack start/size, code start/size, flags, word size, different register sizes etc.
- Run detailed tests using valgrind, get rid of all remaining memory leaks
- Fix inefficient areas of implementation, work on flattening lists/trees into array based implementation

