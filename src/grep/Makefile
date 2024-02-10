CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c11 -pedantic

all: s21_grep

rebuild: clean all

s21_grep:
	clang-format -i s21_grep.c s21_grep.h
	$(CC) $(CFLAGS) s21_grep.c -o s21_grep
clean:
	rm -rf s21_grep

