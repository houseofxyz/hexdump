.PHONY: all hexdump

all: hexdump

hexdump:
	gcc hexdump.c -o hexdump

clean:
	test -f hexdump && rm hexdump || true

