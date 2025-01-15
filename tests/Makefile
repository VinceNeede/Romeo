# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Wno-missing-field-initializers

# Source files
SRCS = token identifier scanner ASTPrinter parser
EXES = $(SRCS:%=../bin/test_%)
# Object files
# SRCS = $(EXES:%=test_%.c)



# Default target
all: $(EXES)

../bin/test_token: test_token.c ../src/token.c
	$(CC) $(CFLAGS) -I../include -o $@ $^ 
../bin/test_identifier: test_identifier.c ../src/token.c
	$(CC) $(CFLAGS)  -I../include -o $@ $^ 
../bin/test_scanner: test_scanner.c ../src/token.c ../src/scanner.c 
	$(CC) $(CFLAGS)  -I../include -o $@ $^ -L../lib/ -lLinkedLists 
../bin/test_ASTPrinter: test_ASTPrinter.c ../src/token.c ../src/Expr.c ../src/ASTPrinter.c
	$(CC) $(CFLAGS) -I../include -o $@ $^ -L../lib/ -lstringutils -lLinkedLists
../bin/test_parser: test_parser.c ../src/token.c ../src/scanner.c ../src/Expr.c ../src/ASTPrinter.c ../src/parser.c
	$(CC) $(CFLAGS) -I../include -o $@ $^ -L../lib/ -lstringutils -lLinkedLists
# Clean up build files
clean:
	rm $(EXES)

# Phony targets
.PHONY: all clean