CC = gcc
CFLAGS = -Wall -Wextra

LLS_SUB = Token char string Rtype Stmt var Expr Literal
LLS_H := $(addprefix ../include/LinkList_, $(addsuffix .h, $(LLS_SUB)))
LLS_C := $(addprefix ./LinkList_, $(addsuffix .c, $(LLS_SUB)))
LLS = $(LLS_H) $(LLS_C)


all: keyword.c libstringutils.a ../bin/generateAST HT_Rtype.c LinkList_Literal.c LinkList_Expr.c LinkList_Stmt.c HT_var.c libLinkedLists.a

../bin/gLL: generate_LinkedList_files.c
	$(CC) $(CFLAGS) -o $@ $^

../bin/gHT: generate_HashTable_files.c
	$(CC) $(CFLAGS) -o $@ $^

keyword.c : keywords.gperf
	gperf -I -L C -G -t -H hash_keywords -N get_keyword -C keywords.gperf > keyword.c

LinkList_Token.c : ../bin/gLL
	../bin/gLL -H ../include -S . -s Token -t Token\* -u -I token.h

LinkList_char.c : ../bin/gLL
	../bin/gLL -H ../include -S . -t char 

LinkList_string.c : ../bin/gLL
	../bin/gLL -H ../include -S . -s string -t char\* -f free 

LinkList_Rtype.c : ../bin/gLL
	../bin/gLL -H ../include -S . -s Rtype -t Rtype\* -u -I Rtype.h

LinkList_Stmt.c : ../bin/gLL
	../bin/gLL -H ../include -S . -s Stmt -t Stmt\* -u -a -I Stmt.h

LinkList_Expr.c : ../bin/gLL
	../bin/gLL -H ../include -S . -s Expr -t Expr\* -u -a -I Expr.h

LinkList_Literal.c : ../bin/gLL
	../bin/gLL -H ../include -S . -s Literal -t Literal\* -u -a -I token.h

HT_Rtype.c : ../bin/gHT LinkList_Rtype.c
	../bin/gHT -H ../include -S . -s Rtype -t Rtype\* -kt char* -kf name -I Rtype.h

LinkList_var.c : ../bin/gLL
	../bin/gLL -H ../include -S . -s var -t Rvariable* -u -f freeRvariable -I variable.h
HT_var.c : ../bin/gHT LinkList_var.c
	../bin/gHT -H ../include -S . -s var -t Rvariable* -kt key_field* -kf key -I variable.h token.h

libstringutils.a : string_utilities.c ../include/string_utilities.h LinkList_string.c LinkList_char.c
	$(CC) $(CFLAGS) -I../include -c string_utilities.c
	ar rcs $@ string_utilities.o
	rm -f string_utilities.o

libLinkedLists.a : $(LLS_C)
	$(CC) $(CFLAGS) -I../include -c $(LLS_C)
	ar rcs $@ $(LLS_C:.c=.o)
	rm -f $(LLS_C:.c=.o)

../bin/generateAST: generateAST.c LinkList_char.c LinkList_string.c
	$(CC) $(CFLAGS) -I../include -o $@ $^ -L./ -lstringutils
	../bin/generateAST ../include/ ../src/

clean:
	rm -f keyword.c $(LLS) ../bin/gLL ../bin/gHT ../bin/generateAST \
	libLinkedLists.a libstringutils.a ../include/Expr.h ../src/Expr.c \
	../src/Stmt.c ../include/Stmt.h HT_Rtype.c ../include/HT_Rtype.h \
	HT_var.c ../include/HT_var.h

.PHONY: all clean
