isa_interpreter:	isa_gen/instruction_set.l isa_gen/instruction_set.y isa_gen/instruction_set.h isa_gen/instruction_set_functions.c isa_gen/system_info_parser.c ast_functions.c
					bison -d isa_gen/instruction_set.y -Wcounterexamples && \
					flex -oinstruction_set.lex.c isa_gen/instruction_set.l && \
					cc -g -o $@ instruction_set.tab.c instruction_set.lex.c isa_gen/instruction_set_functions.c isa_gen/system_info_parser.c ast_functions.c -lm

clean:
	rm -f isa_interpreter && \
	rm -f instruction_set.lex.c && \
	rm -f instruction_set.tab.c && \
	rm -f instruction_set.tab.h

wsl-tkinter:
	sudo apt-get install python3-tk