CC=clang
STND=-ansi
CODEDIR=src/
CFLAGS = $(STND) -std=c99 -Wall -Wextra -g -Wall -pedantic -Wformat=2 -Wshadow \
  -Wno-long-long -Wno-overlength-strings -Wno-format-nonliteral -Wcast-align \
  -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls \
  -Wnested-externs -Wmissing-include-dirs -Wswitch-default

all:
	$(CC) $(CFLAGS) $(CODEDIR)/*.c -ledit -lm -o parsing
clean:
	rm parsing
