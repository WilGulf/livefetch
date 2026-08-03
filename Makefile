CC = cc
CFLAGS = -Wall -Wextra -std=c17 -O2
LDLIBS = -lncurses
LDFLAGS =

PREFIX ?= /usr/local
DATADIR = $(PREFIX)/share/livefetch

CFLAGS += -DDATA_DIR=\"$(DATADIR)\"

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
LDFLAGS += -framework CoreFoundation -framework IOKit -framework CoreGraphics -framework CoreDisplay
DEFAULT_CONF := src/default.conf
else
DEFAULT_CONF := src/default-linux.conf
endif

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

TARGET = livefetch

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) $(LDLIBS) $(LDFLAGS) -o $@
	cp $(DEFAULT_CONF) ./default.conf
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: $(TARGET)
	install -d $(PREFIX)/bin
	install -m 755 $(TARGET) $(PREFIX)/bin/

	install -d $(DATADIR)
	cp -r src/logos $(DATADIR)/
	cp ./default.conf $(DATADIR)/

clean:
	rm -f src/*.o
	rm -r livefetch

.PHONY: all clean
