override CC      := gcc
override CFLAGS  := -I../include -Wall -Wextra -std=gnu11 -O0 -g -Wno-format-extra-args
override LDFLAGS :=
override OBJ_DIR := obj

$(OBJ_DIR)/tests/%.o: $(PROJ_DIR)/tests/%.c
	@echo "\033[1;37m>>> `basename $<`... \033[0m"
	@mkdir -p `dirname $@`
	@$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "\033[1;37m>>> `basename $<`... \033[0m"
	@mkdir -p `dirname $@`
	@$(CC) $(CFLAGS) -c -o $@ $<

all: klist ksorted_array kstring_format

klist: $(OBJ_DIR)/tests/libk/test_klist.o $(OBJ_DIR)/tests/test-common.o $(OBJ_DIR)/libk/klist.o
	@echo "\033[1;37mTesting `basename $@`... \033[0m"
	@$(CC) $(CFLAGS) -o $@ $^
	@./$@
	@rm -f $@

ksorted_array: $(OBJ_DIR)/tests/libk/test_ksorted_array.o $(OBJ_DIR)/tests/test-common.o $(OBJ_DIR)/libk/ksorted_array.o $(OBJ_DIR)/libk/kmemory.o
	@echo "\033[1;37mTesting `basename $@`... \033[0m"
	@$(CC) $(CFLAGS) -o $@ $^
	@./$@
	@rm -f $@

kstring_format: $(OBJ_DIR)/tests/libk/test_kstring_format.o $(OBJ_DIR)/tests/test-common.o $(OBJ_DIR)/libk/kstring_format.o $(OBJ_DIR)/libk/kstring.o $(OBJ_DIR)/libk/kstring_format.o $(OBJ_DIR)/libk/kchar.o $(OBJ_DIR)/libk/kstring_format.o $(OBJ_DIR)/libk/kmemory.o
	@echo "\033[1;37mTesting `basename $@`... \033[0m"
	@$(CC) $(CFLAGS) -o $@ $^
	@./$@
	@rm -f $@

.PHONY: all klist ksorted_array kstring_format
