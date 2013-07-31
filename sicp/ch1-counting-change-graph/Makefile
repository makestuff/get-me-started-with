AMOUNT=11
TYPES=5

all: cc.png
	mv $< $(AMOUNT).png

view: cc.png
	eog $<

cc.png: cc.dot
	dot -Tpng $< -o $@

cc.dot: cc FORCE
	./cc $(AMOUNT) $(TYPES) > $@

cc: cc.c
	gcc -O2 -std=c99 -Wall -Wextra -pedantic-errors -o $@ $<

clean: FORCE
	rm -f cc *.png cc.dot

FORCE:
