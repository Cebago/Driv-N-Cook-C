CC=gcc
CFLAGS= -Wall -Wextra -Werror
dev: build start
# main.c
build:
	$(CC) *.c -o CreateUser `pkg-config --libs --cflags gtk+-3.0` `mysql_config --cflags --libs` -rdynamic -lpng -lcurl

start:
	./CreateUser
