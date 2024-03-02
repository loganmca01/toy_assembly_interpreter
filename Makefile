isa_interpreter:	instruction_set.l instruction_set.y instruction_set.h instruction_set_functions.c
					bison -d instruction_set.y -Wcounterexamples && \
					flex -oinstruction_set.lex.c instruction_set.l && \
					cc -g -o $@ instruction_set.tab.c instruction_set.lex.c instruction_set_functions.c user_environment.c -lm

clean:
	rm -f isa_interpreter

