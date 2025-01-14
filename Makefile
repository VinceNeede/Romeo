CC = gcc
CFLAGS = -Wall -Wextra -g
INCLUDES = -Iinclude -I/home/vince/Tesi/mystring/include/
LIBS = -L/home/vince/Tesi/mystring/ -lmystring
SRC = src/HT_key.c src/identifier_map.c src/LinkList_key.c src/LinkList_Token.c src/scanner.c src/token.c
OBJ = $(SRC:src/%.c=build/%.o)
TARGET = clox

all: $(TARGET)

$(TARGET): $(OBJ) build/clox.o
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

build/clox.o: clox.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ) build/clox.o $(TARGET)

.PHONY: all clean