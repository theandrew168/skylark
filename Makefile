CC = gcc
RM = rm -fr

TARGET = skylark

CFLAGS = -ansi -Wall -Werror -Wextra -pedantic-errors
LDFLAGS = -lSDL2

SRCS = $(wildcard src/*.c)
HEAD = $(wildcard src/*.h)
OBJS = $(patsubst %.c, %.o, $(SRCS))

.PHONY: default all clean run

default: $(TARGET)
all: default

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c $(HEAD)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) $(TARGET)

run: default
	./$(TARGET)
