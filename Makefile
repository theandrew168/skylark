# Request POSIX make behavior and disable any default suffix-based rules
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

# Declare names for build targets: main binary, test binary, static library, shared library
MAIN_BINARY    = skylark
TEST_BINARY    = skylark_tests
STATIC_LIBRARY = libskylark.a
SHARED_LIBRARY = libskylark.so

# Declare which targets should be built by default
all: $(MAIN_BINARY) $(TEST_BINARY) $(STATIC_LIBRARY) $(SHARED_LIBRARY)

# Declare sources, objects, and test sources
sources =         \
  src/chip8.c     \
  src/graphics.c  \
  src/input.c
objects = $(sources:.c=.o)
test_sources =       \
  tests/test_opcode.c

# Declare dependencies between project files
src/file.o: src/file.c src/file.h

# Declare rules for each build target
$(MAIN_BINARY): src/main.c $(STATIC_LIBRARY)
	@echo "BINARY  $@"
	@$(CC) $(CFLAGS) -o $@ src/main.c $(STATIC_LIBRARY) $(LDFLAGS) $(LDLIBS)
$(TEST_BINARY): $(test_sources) tests/main.c $(STATIC_LIBRARY)
	@echo "BINARY  $@"
	@$(CC) $(CFLAGS) -o $@ tests/main.c $(STATIC_LIBRARY) $(LDFLAGS) $(LDLIBS)
$(STATIC_LIBRARY): $(objects)
	@echo "STATIC  $@"
	@$(AR) rcs $@ $(objects)
$(SHARED_LIBRARY): $(objects)
	@echo "SHARED  $@"
	@$(CC) -shared -o $@ $(objects) $(LDFLAGS) $(LDLIBS)

# Default rule for compiling .c files to .o object files
.SUFFIXES: .c .o
.c.o:
	@echo "CC      $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

# Helper target that builds and runs the main binary
.PHONY: run
run: $(MAIN_BINARY)
	./$(MAIN_BINARY)

# Helper target that builds and runs the test binary
.PHONY: check
check: $(TEST_BINARY)
	./$(TEST_BINARY)

# Helper target that cleans up build artifacts
.PHONY: clean
clean:
	rm -f $(MAIN_BINARY) $(TEST_BINARY) $(STATIC_LIBRARY) $(SHARED_LIBRARY) $(objects)
