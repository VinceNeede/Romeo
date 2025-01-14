CC = gcc
CFLAGS = -Wall -Wextra



all: identifier.c ../include/LinkList_Token.h

../bin/gLL: generate_LinkedList_files.c
	$(CC) $(CFLAGS) -o $@ $^

../bin/gHT: generate_HashTable_files.c
	$(CC) $(CFLAGS) -o $@ $^

identifier.c : identifier.gperf
	gperf -I -L C -G -t -H hash_identifier -N get_identifier -C identifier.gperf > identifier.c

../include/LinkList_Token.h : ../bin/gLL
	../bin/gLL -H ../include -S ../src -s Token -t Token* -u -I token.h

clean:
	rm -f identifier.c ../include/LinkList_Token.h ../bin/gLL ../bin/gHT

.PHONY: all clean
