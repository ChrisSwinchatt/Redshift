override CC     := gcc
override CFLAGS += -I ../include -Wall -Wextra -std=gnu11 -O0 -g -Wno-format-extra-args

%.o: %.c
	@$(CC) $(CFLAGS) -c $@ $<

all: klist kksorted_array

clean:

klist: libk/test_klist.o ../libk/klist.o
	@echo "\033[1;37mTesting `basename $@`... \033[0m"
	@$(CC) $(CFLAGS) -o $@ $^

kksorted_array: libk/test_ksorted_array.o ../libk/kksorted_array.o
	@echo "\033[1;37mTesting `basename $@`... \033[0m"
	@$(CC) $(CFLAGS) -o $@ $^
	@./$@
	@rm -f $@ $(subst .c,.o,$^)

.PHONY: all
