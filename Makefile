CC=clang
STND=-ansi
CODEDIR=src/
CFLAGS = $(STND) -std=c99 -Wall

all:
	$(CC) $(CFLAGS) $(CODEDIR)/*.c -ledit -lm -g -o parsing
clean:
	rm parsing
