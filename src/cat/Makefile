CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c11 -pedantic

all: s21_cat

rebuild: clean all

s21_cat:
	clang-format -i s21_cat.c s21_cat.h
	$(CC) $(CFLAGS) s21_cat.c -o s21_cat
clean:
	rm -rf s21_cat

