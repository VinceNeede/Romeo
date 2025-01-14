# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Source files
EXES = token identifier

# Object files
# SRCS = $(EXES:%=test_%.c)



# Default target
all: $(EXES)

token: test_token.c
	$(CC) $(CFLAGS) -I../include -o $@ $^ ../src/token.c
identifier: test_identifier.c
	$(CC) $(CFLAGS) -Wno-missing-field-initializers -I../include -o $@ $^ ../src/token.c

# Clean up build files
clean:
	rm $(EXES)

# Phony targets
.PHONY: all clean