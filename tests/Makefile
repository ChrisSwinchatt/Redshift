CC 	   	:= gcc
CFLAGS 	:= -I ../include -Wall -Wextra -std=gnu11 -O0 -g -Wno-format-extra-args
LDFLAGS :=

%.o: %.c
	@$(CC) $(CFLAGS) -c $@ $<

all: forward_list sorted_array

forward_list: unit/test_forward_list.c ../src/util/forward_list.c
	@echo "\033[1;37mTesting `basename $@`... \033[0m"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@#./$@
	@#rm -f $@ $(subst .c,.o,$^)

sorted_array: unit/test_sorted_array.c ../src/util/sorted_array.c
	@echo "\033[1;37mTesting `basename $@`... \033[0m"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@./$@
	@rm -f $@ $(subst .c,.o,$^)

.PHONY: all
