CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
CPPFLAGS ?= -Iinclude

.PHONY: all clean
all: build/binary_buddy.o

build/binary_buddy.o: src/binary_buddy.c include/binary_buddy.h include/buddy_size.h
	mkdir -p build
	$(CC) $(CPPFLAGS) $(CFLAGS) -c src/binary_buddy.c -o $@

clean:
	rm -rf build
