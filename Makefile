CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2 -D_GNU_SOURCE

.PHONY: all clean
all: build/binary_buddy.o

build/binary_buddy.o: src/binary_buddy.c src/binary_buddy.h src/buddy_size.h
	mkdir -p build
	$(CC) $(CFLAGS) -Isrc -c src/binary_buddy.c -o $@

clean:
	rm -rf build
