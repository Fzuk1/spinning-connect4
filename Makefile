.PHONY: all clean

CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c11 -g
LDFLAGS=-lraylib -lm
TARGET=scn4

all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) $^ -o $@ $(LDFLAGS)

$(TARGET).o: main.c
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f *.o $(TARGET)
