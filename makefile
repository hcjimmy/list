CC = gcc
SRC_DIR = tests
OBJ = $(patsubst %.c, %.o, $(wildcard $(SRC_DIR)/*.c))

.PHONY: test_n_run clean

test_n_run: test
	./test

test: $(OBJ)
	$(CC) -o $@ $^

clean: ; rm $(SRC_DIR)/*.o test

