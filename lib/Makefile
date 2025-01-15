CC = gcc
CFLAGS = -Wall -Wextra

LLS_SUB = Token char string
LLS_H = $(addprefix ../include/LinkList_, $(addsuffix .h, $(LLS_SUB)))
LLS_C = $(addprefix ./LinkList_, $(addsuffix .c, $(LLS_SUB)))
LLS = $(LLS_H) $(LLS_C)

all: identifier.c libLinkedLists.a libstringutils.a ../include/Expr.h

../bin/gLL: generate_LinkedList_files.c
	$(CC) $(CFLAGS) -o $@ $^

../bin/gHT: generate_HashTable_files.c
	$(CC) $(CFLAGS) -o $@ $^

identifier.c : identifier.gperf
	gperf -I -L C -G -t -H hash_identifier -N get_identifier -C identifier.gperf > identifier.c

../include/LinkList_Token.h : ../bin/gLL
	../bin/gLL -H ../include -S . -s Token -t Token* -u -I token.h

../include/LinkList_char.h : ../bin/gLL
	../bin/gLL -H ../include -S . -t char 

../include/LinkList_string.h : ../bin/gLL
	../bin/gLL -H ../include -S . -s string -t char* -f free 

libLinkedLists.a : $(LLS_H)
	$(CC) $(CFLAGS) -I../include -c $(LLS_C)
	ar rcs $@ $(addsuffix .o, $(basename $(LLS_C)))
	rm -f $(addsuffix .o, $(basename $(LLS_C)))

libstringutils.a : string_utilities.c ../include/string_utilities.h libLinkedLists.a
	$(CC) $(CFLAGS) -I../include -c string_utilities.c -L./ -lLinkedLists
	ar rcs $@ string_utilities.o
	rm -f string_utilities.o

../bin/generateAST: generateAST.c libstringutils.a
	$(CC) $(CFLAGS) -I../include -o $@ $^ -L./ -lLinkedLists -lstringutils

../include/Expr.h: ../bin/generateAST
	../bin/generateAST ../include/ ../src/

clean:
	rm -f identifier.c $(LLS) ../bin/gLL ../bin/gHT ../bin/generateAST libLinkedLists.a libstringutils.a ../include/Expr.h ../src/Expr.c

.PHONY: all clean
