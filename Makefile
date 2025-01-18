CC = gcc
CFLAGS = -Wall -Wextra -Wno-missing-field-initializers -Wno-switch
INCLUDES = -Iinclude 
OUT_FOLDER = ~/.local/local_bin/

all: build_lib $(OUT_FOLDER)Romeo

build_lib:
	mkdir -p bin
	@cd lib && make

$(OUT_FOLDER)Romeo: Romeo.c lib/Rtype.c lib/types.c lib/variable.c lib/HT_Rtype.c lib/HT_var.c src/*.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ -Llib -lstringutils -lLinkedLists
clean:
	@cd lib && make clean
	rm -f $(OUT_FOLDER)Romeo

.PHONY: all clean