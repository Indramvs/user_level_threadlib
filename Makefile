# Compiler and flags
CC=gcc
CFLAGS=-Wall -Wextra -g         # Good: enables warnings and debugging symbols

# Object files
OBJ=main.o thread.o mymalloc.o  # List all object files

# Default target
all: mythreads

# Link step
mythreads: $(OBJ)
	$(CC) $(CFLAGS) -o mythreads $(OBJ)

# Cleanup
clean:
	rm -f *.o mythreads
