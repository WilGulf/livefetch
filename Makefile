CC ?= cc
CFLAGS ?= -Wall -Wextra -std=c17 -O2
LDLIBS ?= -lncurses -lm
LDFLAGS ?=

PREFIX ?= /usr/local
DATADIR = $(PREFIX)/share/livefetch

CFLAGS += -DDATA_DIR=\"$(DATADIR)\"

ifeq ($(OS),Windows_NT)
	TARGET_EXT := .exe
	LDLIBS := -lpdcurses_wincon -lwinmm -lm
	DEFAULT_CONF := src/default-windows.conf
else
	UNAME_S := $(shell uname -s)

	ifeq ($(UNAME_S),Darwin)
		LDFLAGS += -framework CoreFoundation
		LDFLAGS += -framework IOKit
		LDFLAGS += -framework CoreGraphics
		LDFLAGS += -framework CoreDisplay
		DEFAULT_CONF := src/default.conf
	else
		DEFAULT_CONF := src/default-linux.conf
	endif
endif

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

TARGET = livefetch$(TARGET_EXT)

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
	rm -r livefetch livefetch.exe

.PHONY: all clean
