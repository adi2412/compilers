# temporary make file. will be improved

all:
	gcc -c -std=c99 lexer.c
	gcc -c -std=c99 first_follow_gen.c
	gcc -c -std=c99 driver.c
	gcc -c -std=c99 parser.c
	gcc -c -std=c99 sem_parser.c 
	gcc -c -std=c99 ast.c 
	gcc -c -std=c99 symbol_table.c
	gcc -o stage1exe driver.o first_follow_gen.o lexer.o parser.o sem_parser.o ast.o symbol_table.o


run:
	./stage1exe test.txt parsetreeOutFile.txt
	
lexer:
	gcc -o lexer lexer.c

parser:
	gcc -o parser parser.c lexer.c

ffgen:
	gcc -o ffgen first_follow_gen.c

driver:
	gcc -o compiler driver.c parser.c lexer.c
