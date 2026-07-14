CC = cc
CFLAGS = -Wall -Wextra -std=c17 -O2
LDLIBS = -lncurses
LDFLAGS =

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    LDFLAGS += -framework CoreFoundation -framework IOKit
endif

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

TARGET = livefetch

all: $(TARGET)
	

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(OBJ) -o $@
	#cp -r src/logos ./
	cp src/default.conf ./
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o
	rm -f default.conf
	rm -rf logos
	rm -r livefetch

.PHONY: all clean