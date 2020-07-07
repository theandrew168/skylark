# Request POSIX Make behavior and disable any default suffix-based rules
.POSIX:
.SUFFIXES:


# Declare compiler tools and flags
AR      = ar
CC      = cc
CFLAGS  = -std=c99
CFLAGS += -fPIC -g -Og
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wno-unused-parameter
CFLAGS += -Isrc/ -I/usr/include/SDL2
LDFLAGS =
LDLIBS  = -lSDL2


# Declare which targets should be built by default
default: skylark skylark_tests
all: libskylark.a libskylark.so skylark skylark_tests dis rom2c


# Declare static / shared library sources
libskylark_sources =  \
  src/chip8.c         \
  src/instruction.c
libskylark_objects = $(libskylark_sources:.c=.o)

# Express dependencies between object and source files
src/chip8.o: src/chip8.c src/chip8.h src/instruction.h
src/instruction.o: src/instruction.c src/instruction.h

# Build the static library
libskylark.a: $(libskylark_objects)
	@echo "STATIC  $@"
	@$(AR) rcs $@ $(libskylark_objects)

# Build the shared library
libskylark.so: $(libskylark_objects)
	@echo "SHARED  $@"
	@$(CC) $(LDFLAGS) -shared -o $@ $(libskylark_objects) $(LDLIBS)


# Build the main binary
skylark: src/main.c libskylark.a
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/main.c libskylark.a $(LDLIBS)


# Build the tests binary
skylark_tests_sources =  \
  src/chip8_test.c       \
  src/instruction_test.c

skylark_tests: $(skylark_tests_sources) src/main_test.c libskylark.a
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) -o $@ src/main_test.c libskylark.a


# Build the disassembler
dis: tools/dis.c libskylark.a
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) -o $@ tools/dis.c libskylark.a


# Build the ROM to C conversion tool
rom2c: tools/rom2c.c
	@echo "EXE     $@"
	@$(CC) $(CFLAGS) -o $@ tools/rom2c.c


# Helper target that builds and runs the test binary
.PHONY: check
check: skylark_tests
	./skylark_tests

# Helper target that cleans up build artifacts
.PHONY: clean
clean:
	rm -fr skylark skylark_tests dis rom2c *.a *.so src/*.o


# Default rule for compiling .c files to .o object files
.SUFFIXES: .c .o
.c.o:
	@echo "CC      $@"
	@$(CC) $(CFLAGS) -c -o $@ $<
