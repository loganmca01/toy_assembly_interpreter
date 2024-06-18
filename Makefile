isa_interpreter:	isa_gen/instruction_set.l isa_gen/instruction_set.y isa_gen/instruction_set.h isa_gen/instruction_set_functions.c isa_gen/system_info_parser.c ast_functions.c
					bison -oisa_gen/instruction_set.tab.c -d isa_gen/instruction_set.y -Wcounterexamples && \
					flex -oisa_gen/instruction_set.lex.c isa_gen/instruction_set.l && \
					cc -g -o $@ isa_gen/instruction_set.tab.c isa_gen/instruction_set.lex.c isa_gen/instruction_set_functions.c isa_gen/system_info_parser.c ast_functions.c -lm

vm_gen: vm/vm_main.c vm/vm_parser.y vm/vm_parser.l vm/vm.h ast_functions.c vm/vm_parser_functions.c vm/vm_ipc.c
		bison -ovm/vm_parser.tab.c -d vm/vm_parser.y -Wcounterexamples && \
		flex -ovm/vm_parser.lex.c vm/vm_parser.l && \
		cc -g -o $@ vm/vm_parser.tab.c vm/vm_parser.lex.c vm/vm_main.c ast_functions.c vm/vm_parser_functions.c vm/vm_ipc.c -lm

clean:
	rm -f isa_interpreter && \
	rm -f isa_gen/instruction_set.lex.c && \
	rm -f isa_gen/instruction_set.tab.c && \
	rm -f isa_gen/instruction_set.tab.h && \
	rm -f vm/vm_parser.lex.c && \
	rm -f vm/vm_parser.tab.c && \
	rm -f vm/vm_parser.tab.h && \
	rm -f vm_gen

wsl-tkinter:
	sudo apt-get install python3-tk