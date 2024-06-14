# toy_assembly_interpreter
Customizable interpreter for toy assembly languages for use in computer organization classes.

# Current Status

isa file generation version 1 is complete. Working now on building the virtual machine based on isa files.

# How to use (so far)

isa file generation:

make isa_interpreter

./isa_interpreter [system file] [instruction set file] [output file (.isa)]

VM:

make vm_gen

./vm_gen [.isa file]

currently only prints the loaded system information after parsing, working on the full VM.
