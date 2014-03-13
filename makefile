# temporary make file. will be improved

lexer:
	gcc -o lexer lexer.c

parser:
	gcc -o parser parser.c lexer.c

ffgen:
	gcc -o ffgen first_follow_gen.c

driver:
	gcc -o compiler driver.c parser.c lexer.c