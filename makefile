# temporary make file. will be improved

all:
	gcc -c -std=c99 lexer.c
	gcc -c -std=c99 first_follow_gen.c
	gcc -c -std=c99 driver.c
	gcc -c -std=c99 parser.c
	gcc -c -std=c99 sem_parser.c 
	gcc -c -std=c99 ast.c 
	gcc -c -std=c99 symbol_table.c
	gcc -c -std=c99 type_extractor.c
	gcc -c -std=c99 semantic_analyzer.c
	gcc -o stage1exe driver.o first_follow_gen.o lexer.o parser.o sem_parser.o ast.o symbol_table.o type_extractor.o semantic_analyzer.o


run_test1:
	./stage1exe testcase1.txt parsetreeOutFile.txt

run_test2:
	./stage1exe testcase2.txt parsetreeOutFile.txt

run_test3:
	./stage1exe testcase3.txt parsetreeOutFile.txt

run_test4:
	./stage1exe testcase4.txt parsetreeOutFile.txt

run_test5:
	./stage1exe testcase5.txt parsetreeOutFile.txt

run_test6:
	./stage1exe testcase6.txt parsetreeOutFile.txt
