# Request POSIX Make behavior and disable any default suffix-based rules
.POSIX:
.SUFFIXES:


# Declare compiler tools and flags
AR      = ar
CC      = cc
CFLAGS  = -std=c99 -D_POSIX_C_SOURCE=200112L
CFLAGS += -fPIC -g -Og
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wno-unused
CFLAGS += -Isrc/
LDFLAGS =
LDLIBS  = -lSDL2


# Declare which targets should be built by default
all: libskylark.a libskylark.so skylark skylark_tests dis rom2c


# Declare static / shared library sources
libskylark_sources =  \
  src/chip8.c         \
  src/file.c          \
  src/graphics.c      \
  src/input.c         \
  src/isa.c
libskylark_objects = $(libskylark_sources:.c=.o)

# Express dependencies between object and source files
src/chip8.o: src/chip8.c src/chip8.h
src/file.o: src/file.c src/file.h
src/graphics.o: src/graphics.c src/graphics.h
src/input.o: src/input.c src/input.h
src/isa.o: src/isa.c src/isa.h

# Build the static library
libskylark.a: $(libskylark_objects)
	@echo "STATIC  $@"
	@$(AR) rcs $@ $(libskylark_objects)

# Build the shared library
libskylark.so: $(libskylark_objects)
	@echo "SHARED  $@"
	@$(CC) -shared -o $@ $(libskylark_objects) $(LDFLAGS) $(LDLIBS)


# Build the main binary
skylark: src/main.c libskylark.a rom2c
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) -o $@ src/main.c libskylark.a $(LDFLAGS) $(LDLIBS)


# Build the tests binary
skylark_tests_sources =  \
  src/isa_test.c

skylark_tests: $(skylark_tests_sources) src/main_test.c libskylark.a
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) -o $@ src/main_test.c libskylark.a $(LDFLAGS) $(LDLIBS)


# Build the disassembler
dis: tools/dis.c libskylark.a
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) -o $@ tools/dis.c libskylark.a $(LDFLAGS) $(LDLIBS)


# Build the ROM to C conversion tool
rom2c: tools/rom2c.c
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) -o $@ tools/rom2c.c $(LDFLAGS) $(LDLIBS)


# Helper target that builds and runs the main binary
.PHONY: run
run: skylark
	./skylark

# Helper target that builds and runs the test binary
.PHONY: check
check: skylark_tests
	./skylark_tests

# Helper target that cleans up build artifacts
.PHONY: clean
clean:
	rm -f libskylark.a libskylark.so skylark skylark_tests dis rom2c $(libskylark_objects)


# Default rule for compiling .c files to .o object files
.SUFFIXES: .c .o
.c.o:
	@echo "CC      $@"
	@$(CC) $(CFLAGS) -c -o $@ $<
