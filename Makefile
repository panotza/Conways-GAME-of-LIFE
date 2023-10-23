CC=gcc
CFLAGS+=-O3 -Wall -Wextra -pedantic

OUTPUT=main

$(OUTPUT): main.c
	$(CC) $(CFLAGS) -o $(OUTPUT) main.c

clean:
	rm -f $(OUTPUT)