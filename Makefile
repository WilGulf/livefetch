CC = cc
CFLAGS = -Wall -Wextra -std=c17 -O2

TARGET = livefetch
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)