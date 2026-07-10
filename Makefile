CC = cc
CFLAGS = -Wall -Wextra -std=c17 -O2 -lncurses

TARGET = livefetch
SRC = main.c config.c common.c sysinfo.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)