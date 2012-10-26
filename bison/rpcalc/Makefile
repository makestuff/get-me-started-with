all: rpcalc

rpcalc: main.o rpcalc.tab.o
	gcc -g -o $@ $+ -lm

%.o: %.c rpcalc.tab.h
	gcc -g -c -Wall -pedantic-errors -std=c99 -o $@ $<

rpcalc.tab.c rpcalc.tab.h: rpcalc.y
	bison --defines rpcalc.y

clean: FORCE
	rm -f rpcalc.tab.* *.o rpcalc.output rpcalc

FORCE:
