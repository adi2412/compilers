# temporary make file. will be improved

lexer:
	gcc -o lexer lexer.c

parser:
	gcc -o parser parser.c lexer.c

driver:
	gcc -o compiler driver.c parser.c lexer.c