OBJ_DIR := $(PWD)/../obj/libk
SOURCES := $(shell find -name "*.c")
OBJECTS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SOURCES))
BINARY  := $(LIB_DIR)/libk.a

kabi_stack_guard.o: CFLAGS := $(filter-out -fstack-protector-all,$(CFLAGS)) -fno-stack-protector

%.o: %.c
	@echo "\033[1;37mCompiling `basename $<`... \033[0m"
	@$(CC) $(CFLAGS) -c -o $@ $<

all: $(BINARY)

clean:
	@echo "\033[1;37mCleaning libk... \033[0m"
	@rm -f $(OBJECTS) $(BINARY)

$(BINARY): $(OBJECTS)
	@echo "\033[1;37mLinking `basename $@`"
	@ar -cvq $@ $^

.PHONY: all clean $(BINARY)
