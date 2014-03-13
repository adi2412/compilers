# temporary make file. will be improved

lexer:
	rm lexer && gcc -o lexer lexer.c

parser:
	rm parser && gcc -o parser parser.c lexer.c

ffgen:
	rm ffgen && gcc -o ffgen first_follow_gen.c

driver:
	gcc -o compiler driver.c parser.c lexer.c