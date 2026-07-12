CC = cc
CFLAGS = -Wall -Wextra -std=c17 -O2 -lncurses

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

TARGET = livefetch

all: $(TARGET)
	

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@
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