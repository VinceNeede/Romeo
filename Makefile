CC = gcc
CFLAGS = -Wall -Wextra -Wno-missing-field-initializers
INCLUDES = -Iinclude 

all: build_lib build_tests Romeo

build_lib:
	mkdir -p bin
	@cd lib && make

build_tests: build_lib
	@cd tests && make

Romeo: 
	$(CC) $(CFLAGS) $(INCLUDES) -o Romeo Romeo.c src/*.c -Llib -lstringutils -lLinkedLists
clean:
	@cd lib && make clean
	@cd tests && make clean
	rm -f Romeo

.PHONY: all clean