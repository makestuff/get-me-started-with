CFLAGS := -m64 -fPIC -Dbool=char -I/usr/lib/perl/5.14.2/CORE -I. -DLINUX=2 -D_REENTRANT -D_LARGEFILE64_SOURCE -shared
MODULE := myret

all: $(MODULE).so
	./$(MODULE).pl

$(MODULE).so: $(MODULE)_wrap.c
	gcc $(CFLAGS) $< -o $@

$(MODULE)_wrap.c: $(MODULE).i
	swig -perl5 $<

clean: FORCE
	rm -f *.so *.pm *_wrap.c

FORCE:
