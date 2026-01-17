bf: main.c lex.c
	cc $^ -o $@

run: bf
	./bf
