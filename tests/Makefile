CC 	   	:= gcc
CFLAGS 	:= -I ../include -Wall -Wextra -std=gnu11 -O0 -g -Wno-format-extra-args
LDFLAGS :=

%.o: %.c
	@$(CC) $(CFLAGS) -c $@ $<

all: bin/sorted_array

bin/sorted_array: unit/test_sorted_array.c ../src/util/sorted_array.c
	@echo "\033[1;37mTesting `basename $@`... \033[0m"
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@#$@
	@#echo rm -f $@ $(subst .c,.o,$^)
.PHONY: all
