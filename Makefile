# ================================================================
# Makefile — Book Record Management System
# ================================================================
# USAGE:
#   make          → compile the project
#   make run      → compile and run
#   make clean    → remove compiled files and data
# ================================================================

CC     = gcc
CFLAGS = -Wall -ansi -pedantic
TARGET = BookSystem
SRCS   = main.c book.c
OBJS   = $(SRCS:.c=.o)

# Default target: build the executable
all: $(TARGET)

# Link object files into final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile each .c file into a .o object file
# '$<' = the .c source file, '$@' = the .o output file
main.o: main.c book.h
	$(CC) $(CFLAGS) -c main.c -o main.o

book.o: book.c book.h
	$(CC) $(CFLAGS) -c book.c -o book.o

# Compile and immediately run
run: $(TARGET)
	./$(TARGET)

# Remove generated files
clean:
	rm -f $(OBJS) $(TARGET) books.dat

.PHONY: all run clean
