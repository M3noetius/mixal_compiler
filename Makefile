CC=gcc
BISON=bison
FLEX=flex
SRC=src
CFLAGS=-lfl
MAKE=make

.ONESHELL:
build: 
	cd $(SRC)
	$(BISON) -d parse.y
	$(FLEX) lex.l 
	$(CC) -o ../compiler parse.tab.c lex.yy.c helper_funcs.c symbols.c $(CFLAGS)

.ONESHELL:
build_debug:
	cd $(SRC)
	cp parse.y debug/parse.y
	cp debug/parse_debug.y parse.y
	
	cd ../
	$(MAKE) build
	cd $(SRC)
	mv debug/parse.y parse.y

clean:
	rm compiler
	rm $(SRC)/lex.yy.c
	rm $(SRC)/parse.tab.c
	rm $(SRC)/parse.tab.h


test:
	bash test_mixal.sh 
