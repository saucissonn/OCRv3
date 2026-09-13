CC = gcc

TARGET = n
TEST_TARGET = test_run

SRC = $(shell find src -name '*.c')
TEST_SRC = $(shell find tests -name '*.c')

# Pour les tests, on prend tout src sauf le main normal.
SRC_NO_MAIN = $(filter-out src/main.c,$(SRC))

OBJ = $(patsubst src/%.c,build/src/%.o,$(SRC))
TEST_OBJ = $(patsubst src/%.c,build/src/%.o,$(SRC_NO_MAIN))
TEST_OBJ += $(patsubst tests/%.c,build/tests/%.o,$(TEST_SRC))

CFLAGS = -Iinclude -O3 -march=native -fsanitize=address -g
LDFLAGS = -fsanitize=address -lSDL2_ttf -lSDL2_image -lSDL2 -lm

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)


test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJ)
	$(CC) $(TEST_OBJ) -o $(TEST_TARGET) $(LDFLAGS)

build/src/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/tests/%.o: tests/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	LSAN_OPTIONS=suppressions=lsan.supp:print_suppressions=1 ./$(TARGET)

clean:
	rm -rf build
	rm -f $(TARGET) $(TEST_TARGET)

re: clean all

retest: clean test

.PHONY: all test run clean re retest
