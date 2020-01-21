
vpath %.c src

MKDIR := mkdir -p
RM := rm -f

CC := gcc
CFLAGS := -std=c17 -Wall -Wextra -Wpedantic -Werror -O1 -g3 -ggdb
CPPFLAGS := -Iinclude -D_GNU_SOURCE -D_POSIX_SOURCE
LDFLAGS := 
LIBS :=

OBJS := $(patsubst %.c,%.o,$(notdir $(wildcard src/*.c)))

TARGET := xoracle

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: $.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	$(RM) $(OBJS) $(TARGET)
