CC = gcc
CFLAGS = -Wall -Wextra

LLS_SUB = Token char string Stmt var
LLS_H := $(addprefix ../include/LinkList_, $(addsuffix .h, $(LLS_SUB)))
LLS_C := $(addprefix ./LinkList_, $(addsuffix .c, $(LLS_SUB)))
LLS = $(LLS_H) $(LLS_C)


all: identifier.c libstringutils.a ../bin/generateAST LinkList_Stmt.c HT_string.c HT_var.c libLinkedLists.a

../bin/gLL: generate_LinkedList_files.c
	$(CC) $(CFLAGS) -o $@ $^

../bin/gHT: generate_HashTable_files.c
	$(CC) $(CFLAGS) -o $@ $^

identifier.c : identifier.gperf
	gperf -I -L C -G -t -H hash_identifier -N get_identifier -C identifier.gperf > identifier.c

LinkList_Token.c : ../bin/gLL
	../bin/gLL -H ../include -S . -s Token -t Token* -u -I token.h

LinkList_char.c : ../bin/gLL
	../bin/gLL -H ../include -S . -t char 

LinkList_string.c : ../bin/gLL
	../bin/gLL -H ../include -S . -s string -t char* -f free 

LinkList_Stmt.c : ../bin/gLL
	../bin/gLL -H ../include -S . -s Stmt -t Stmt* -u -I Stmt.h

HT_string.c : ../bin/gHT
	../bin/gHT -H ../include -S . -s string -t char*

LinkList_var.c : ../bin/gLL
	../bin/gLL -H ../include -S . -s var -t Rvariable* -u -f freeRvariable -I variable.h
HT_var.c : ../bin/gHT LinkList_var.c
	../bin/gHT -H ../include -S . -s var -t Rvariable* -kt Token* -kf name -I variable.h token.h

libstringutils.a : string_utilities.c ../include/string_utilities.h LinkList_string.c LinkList_char.c
	$(CC) $(CFLAGS) -I../include -c string_utilities.c
	ar rcs $@ string_utilities.o
	rm -f string_utilities.o

libLinkedLists.a : $(LLS_H)
	$(CC) $(CFLAGS) -I../include -c $(LLS_C)
	ar rcs $@ $(LLS_C:.c=.o)
	rm -f $(LLS_C:.c=.o)

../bin/generateAST: generateAST.c LinkList_char.c LinkList_string.c LinkList_Token.c
	$(CC) $(CFLAGS) -I../include -o $@ $^ ../src/token.c -L./ -lstringutils
	../bin/generateAST ../include/ ../src/

clean:
	rm -f identifier.c $(LLS) ../bin/gLL ../bin/gHT ../bin/generateAST \
	libLinkedLists.a libstringutils.a ../include/Expr.h ../src/Expr.c \
	../src/Stmt.c ../include/Stmt.h HT_string.c ../include/HT_string.h \
	HT_var.c ../include/HT_var.h

.PHONY: all clean
