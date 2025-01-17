CC = gcc
CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -Wno-switch
INCLUDES = -Iinclude 
OUT_FOLDER = ~/.local/local_bin/

all: build_lib build_tests Romeo

build_lib:
	mkdir -p bin
	@cd lib && make

build_tests: build_lib
	@cd tests && make

Romeo: Romeo.c lib/types.c lib/variable.c lib/HT_string.c lib/HT_var.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $(OUT_FOLDER)$@ $^ src/*.c -Llib -lstringutils -lLinkedLists
clean:
	@cd lib && make clean
	@cd tests && make clean
	rm -f Romeo

.PHONY: all clean