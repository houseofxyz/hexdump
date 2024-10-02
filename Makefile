.PHONY: all hexdump debug

all: hexdump debug

hexdump:
	gcc hexdump.c -o hexdump

debug:
	gcc -DDEBUG hexdump.c -o hexdump_d

clean:
	test -f hexdump && rm hexdump || true
	test -f hexdump_d && rm hexdump_d || true

