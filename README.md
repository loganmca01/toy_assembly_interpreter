# Educational Instruction Set Emulator

A tool for teaching students about instruction sets and processor architecture.


## Notes on Current State of Project

As of November 2025 this project is being worked on again. I'm working on organizing the prior work, and trying to figure out what I want to carry over from past attempts. This project grew in scope repeatedly the last time it was in development, as explained below.

### Original plans, and how they changed

This began as a summer project while I was working as a research assistant for Professor Pfaffmann at Lafayette College. I wanted to make a program that could help teach computer organization classes, and show off how assembly instructions actually impacted processor state in a number of different ISAs, both real and not. The core of the design was always the dynamic instruction sets; the program needed to be able to simulate multiple different ISAs based off of a definition. 

The project started as a simple assembly interpreter, reading plaintext assembly instructions and modifying memory/registers. As I was working on this version, I felt strongly that I was missing a lot of important details for learning in ignoring machine code, so I decided that I wanted to also define how instructions are encoded into binary, and have the actual simulator run that.

Problems with this new plan:
1. How do I define machine encodings? Different ISAs encode instructions incredibly differently, is it possible to created a unified definition language?
	- I was stuck on this for a while, and it was only through later work with gem5 and digging into their ISA subsystem that I began to see a solution.
2. Need a dynamic assembler that also works based off of an ISA definition.

The remainder of that summer was spent on these issues, and a deep dive into x86, ARM, and RISC-V binary encodings. The project was then put on hold.

### Plans going forward

In working with gem5, I've gained an understanding of how their ISA system works, and the domain-specific language that they use for definitions. 

