CC = gcc
CFLAGS = -Wall -Werror -g
OBJS = assembler.o test_assembler.o
TARGET = test_assembler

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
