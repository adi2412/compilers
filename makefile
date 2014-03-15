# temporary make file. will be improved

all:
	gcc -c lexer.c
	gcc -c first_follow_gen.c
	gcc -c driver.c
	gcc -o stage1exe driver.o first_follow_gen.o lexer.o

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