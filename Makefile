CC = gcc
CFLAGS = -Wall -Wextra -std=c11
EXEC = tema1
SRC = main.c

all: build

build: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

clean:
	rm -f $(EXEC)
