export VERSION_MAJOR  := 0
export VERSION_MINOR  := 1
export PREFIX         := i686-linux-gnu
export KERNEL_NAME    := redshift
export KERNEL_VERSION := $(VERSION_MAJOR).$(VERSION_MINOR)
export INCLUDE_DIR    := $(PWD)/include $(PWD)/include/libc 
export OUTPUT_DIR     := $(PWD)/out
export INITRD_DIR     := $(OUTPUT_DIR)/initrd
export ISO_DIR        := $(OUTPUT_DIR)/isofs
export LIB_DIR        := $(OUTPUT_DIR)/lib

DEFINES               := -DKERNEL="$(KERNEL_NAME)" -DVERSION_MAJOR="$(VERSION_MAJOR)"                                  \
                         -DVERSION_MINOR="$(VERSION_MINOR)"\ -DVERSION_STR="\"$(KERNEL_VERSION)\""

INCLUDES 			  := -I$(PWD)/include

export AFLAGS         :=
export CC             := $(PREFIX)-gcc
export CFLAGS         := -Wall -Wextra -Werror -std=gnu11 -O2 -ffreestanding -fno-stack-protector -nostdlib	\
                         -fno-omit-frame-pointer $(INCLUDES) $(DEFINES) -no-pie
export LDFLAGS        := -Ttools/kernel.ld -nostdlib -L$(LIB_DIR)

OBJECTS               := obj/abi/crti.o\
						 $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)\
						 $(patsubst src/%.S,obj/%.o,$(shell find src -name "*.S" ! -name "crti.S" ! -name "crtn.S"))\
						 $(patsubst src/%.c,obj/%.o,$(shell find src -name "*.c"))\
						 $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)\
						 obj/abi/crtn.o 
LIBRARIES             := -lgcc
KERNEL                := $(ISO_DIR)/boot/$(KERNEL_NAME)-kernel-$(KERNEL_VERSION)
INITRD                := $(ISO_DIR)/boot/$(KERNEL_NAME)-initrd-$(KERNEL_VERSION)
MAP                   := $(INITRD_DIR)/$(KERNEL_NAME).map
IMAGE                 := $(OUTPUT_DIR)/$(KERNEL_NAME).iso
DEBUG_BIN             := $(OUTPUT_DIR)/$(KERNEL_NAME)-kernel-$(KERNEL_VERSION).debug

ifneq ($(DEBUG),1)
CFLAGS += -g
else
CFLAGS += -DNDEBUG
endif

obj/libk/kabi_stack_guard.o: CFLAGS := $(filter-out -fstack-protector-all,$(CFLAGS)) -fno-stack-protector

obj/%.o: src/%.S
	@echo "\033[1;37m>>> `basename $<`... \033[0m"
	@mkdir -p `dirname $@`
	@$(CC) $(AFLAGS) $(CFLAGS) -c -o $@ $<

obj/%.o: src/%.c
	@echo "\033[1;37m>>> `basename $<`... \033[0m"
	@mkdir -p `dirname $@`
	@$(CC) $(CFLAGS) -c -o $@ $<

all: image

clean:
	@echo "\033[1;37mCleaning redshift... \033[0m"
	@find obj/ -name *.o -exec rm {} \;
	@rm -f $(KERNEL) $(MAP) $(DEBUG_BIN)

image: $(IMAGE)

kernel: $(KERNEL)

initrd: $(INITRD)

$(IMAGE): initrd
	@echo "\033[1;37mCreating `basename $@`... \033[0m"
	@mkdir -p `dirname $@`
	@grub-mkrescue -o $(IMAGE) out/isofs #2>/dev/null

$(KERNEL): $(CRTI) $(CRTBEGIN) $(OBJECTS) $(CRTEND) $(CRTN)
	@echo "\033[1;37m>>> `basename $@`... \033[0m"
	@mkdir -p `dirname $@`
	@$(CC) $(CFLAGS) $(LDFLAGS) -o "$@" $^ $(LIBRARIES)
	@echo "\033[1;37m>>> $(MAP)... \033[0m"
	@mkdir -p `dirname $(MAP)`
	@tools/gensymtab "$@" "$(MAP)"
	@echo "\033[1;37m>>> `basename $(DEBUG_BIN)`... \033[0m"
	@mkdir -p `dirname $(DEBUG_BIN)`
	@cp "$@" "$(DEBUG_BIN)"
	@strip --strip-all "$@"
	@mkdir -p out/isofs/boot/grub
	@cat tools/grub.cfg >out/isofs/boot/grub/grub.cfg

$(INITRD): kernel
	@echo "\033[1;37mGenerating initial ramdisk... \033[0m"
	@mkdir -p `dirname $@`
	@rm -f $@
	@cd $(INITRD_DIR) && tar -cf "$@" * >/dev/null

test:
	$(MAKE) -s -C tests

doc:
	@echo "\033[1;37mGenerating documentation... \033[0m"
	doxygen Doxyfile

run-qemu:
	@export DISPLAY=":0" ; qemu-system-i386 -cdrom "$(IMAGE)" -boot d -monitor stdio

debug-qemu:
	@export DISPLAY=":0" ; qemu-system-i386 -cdrom "$(IMAGE)" -boot d -s -S -monitor stdio

statistics:
	@tools/kstats

analyse:
	@cppcheck --quiet --enable=all $(INCLUDES) `find -name "*.c"` `find -name "*.h"` 2>&1 | grep -v "never used"

xgcc:
	@cd out && ../tools/build-xgcc $(ARGS) $(TARGET)

.PHONY: all image $(IMAGE) kernel $(KERNEL) initrd $(INITRD) tools doc commit run-qemu debug-qemu statistics analyse unit
