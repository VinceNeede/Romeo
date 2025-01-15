# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Source files
SRCS = token identifier scanner ASTPrinter
EXES = $(SRCS:%=../bin/test_%)
# Object files
# SRCS = $(EXES:%=test_%.c)



# Default target
all: $(EXES)

../bin/test_token: test_token.c ../src/token.c
	$(CC) $(CFLAGS) -I../include -o $@ $^ 
../bin/test_identifier: test_identifier.c ../src/token.c
	$(CC) $(CFLAGS) -Wno-missing-field-initializers -I../include -o $@ $^ 
../bin/test_scanner: test_scanner.c ../src/token.c ../src/scanner.c 
	$(CC) $(CFLAGS) -Wno-missing-field-initializers -I../include -o $@ $^ -L../lib/ -lLinkedLists 
../bin/test_ASTPrinter: test_ASTPrinter.c ../src/token.c ../src/Expr.c ../src/ASTPrinter.c
	$(CC) $(CFLAGS) -I../include -o $@ $^ -L../lib/ -lstringutils -lLinkedLists
# Clean up build files
clean:
	rm $(EXES)

# Phony targets
.PHONY: all clean