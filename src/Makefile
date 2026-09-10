all:
	gcc -o n $(shell find . -name '*.c') \
		-lSDL2_ttf -lSDL2_image -lSDL2 -lm -O3 -march=native

run:
	LSAN_OPTIONS=suppressions=lsan.supp:print_suppressions=1 ./n
