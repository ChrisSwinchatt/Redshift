export VERSION_MAJOR  := 0
export VERSION_MINOR  := 1
export ARCH           := i686
export TARGET         := $(ARCH)-elf
export PREFIX         := $(TARGET)
export KERNEL_NAME    := redshift
export KERNEL_VERSION := $(VERSION_MAJOR).$(VERSION_MINOR)
export ARCH_DIR       := $(PWD)/arch/$(ARCH)
export INCLUDE_DIR    := $(PWD)/include $(PWD)/include/libc $(PWD)/include/arch/$(ARCH)
export OUTPUT_DIR     := $(PWD)/out
export INITRD_DIR     := $(OUTPUT_DIR)/initrd
export ISO_DIR        := $(OUTPUT_DIR)/isofs
export LIB_DIR        := $(OUTPUT_DIR)/lib

DEFINES               := -DKERNEL="$(KERNEL_NAME)" -DVERSION_MAJOR="$(VERSION_MAJOR)"                                  \
                         -DVERSION_MINOR="$(VERSION_MINOR)"\ -DVERSION_STR="\"$(KERNEL_VERSION)\"" -DARCH="\"$(ARCH)\""

ifneq ($(DEBUG),1)
DEFINES += -g
else
DEFINES += -DNDEBUG
endif

INCLUDES 			  := -I$(PWD)/include -I$(PWD)/include/libc -I$(PWD)/include/arch/$(ARCH)

export AFLAGS         :=
export CC             := $(PREFIX)-g++
export CFLAGS         := -Wall -Wextra -Werror -std=c++17 -O2 -ffreestanding -fstack-protector-all -nostdlib	\
                         -fno-omit-frame-pointer -fno-exceptions -fno-rtti $(INCLUDES) $(DEFINES) $(DEBUG)
export LDFLAGS        := -Ttools/kernel.ld -nostdlib -L$(LIB_DIR)

CRTI                  := $(ARCH_DIR)/abi/crti.o
CRTBEGIN              := $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND                := $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN                  := $(ARCH_DIR)/abi/crtn.o
SOURCES               := $(shell find $(ARCH_DIR) -name "*.S" ! -name "crti.S" ! -name "crtn.S")\
						 $(shell find $(ARCH_DIR) src/ -name "*.cpp")
OBJECTS               := $(subst .S,.o,$(subst .cpp,.o,$(SOURCES)))
LIBRARIES             := -lk -lgcc
KERNEL                := $(ISO_DIR)/boot/$(KERNEL_NAME)-kernel-$(ARCH)-$(KERNEL_VERSION)
INITRD                := $(ISO_DIR)/boot/$(KERNEL_NAME)-initrd-$(ARCH)-$(KERNEL_VERSION)
MAP                   := $(INITRD_DIR)/$(KERNEL_NAME).map
IMAGE                 := $(OUTPUT_DIR)/$(KERNEL_NAME)-$(ARCH).iso
DEBUG                 := $(OUTPUT_DIR)/$(KERNEL_NAME)-kernel-$(ARCH)-$(KERNEL_VERSION).debug

%.o: %.S
	@echo "\033[1;37mAssembling `basename $<`... \033[0m"
	@$(CC) $(AFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	@echo "\033[1;37mCompiling `basename $<`... \033[0m"
	@$(CC) $(CFLAGS) -c -o $@ $<

all: image

clean:
	@echo "\033[1;37mCleaning redshift... \033[0m"
	@rm -f $(OBJECTS)
	@$(MAKE) -s -C libk clean
	@$(MAKE) -s -C tests clean

image: $(IMAGE)

libk:
	@$(MAKE) -s -C libk

kernel: libk $(KERNEL)

initrd: $(INITRD)

$(IMAGE): initrd
	@echo "\033[1;37mCreating `basename $@`... \033[0m"
	@grub-mkrescue -o $(IMAGE) out/isofs #2>/dev/null

$(KERNEL): $(CRTI) $(CRTBEGIN) $(OBJECTS) $(CRTEND) $(CRTN)
	@echo "\033[1;37mLinking `basename $@`... \033[0m"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o "$@" $^ $(LIBRARIES)
	@echo "\033[1;37mGenerating symbol table... \033[0m"
	@tools/gensymtab "$@" "$(MAP)"
	@echo "\033[1;37mCreating debug file `basename $(DEBUG)`... \033[0m"
	@cp "$@" "$(DEBUG)"
	@echo "\033[1;37mStripping `basename $@`... \033[0m"
	@strip --strip-all "$@"

$(INITRD): kernel
	@echo "\033[1;37mGenerating initial ramdisk... \033[0m"
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
	@export DISPLAY=":0" ; qemu-system-i386 -cdrom "$(IMAGE)" -boot d -s -S &
	@gdb -s "$(DEBUG)" -q -ex "target remote localhost:1234" -ex "b hang"
statistics:
	@tools/kstats

analyse:
	@cppcheck --quiet --enable=all $(INCLUDES) `find -name "*.c"` `find -name "*.h"` 2>&1 | grep -v "never used"

xgcc:
	@cd out && ../tools/build-xgcc $(ARGS) $(TARGET)

.PHONY: all image $(IMAGE) libk kernel $(KERNEL) initrd $(INITRD) tools doc commit run-qemu debug-qemu statistics analyse unit
