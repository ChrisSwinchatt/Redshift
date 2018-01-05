export VERSION_MAJOR := 0
export VERSION_MINOR := 1
ARCH                 := i686
TARGET               := $(ARCH)-elf
PREFIX               := $(TARGET)
KERNEL_NAME          := redshift
VERSION              := $(VERSION_MAJOR).$(VERSION_MINOR)
AS                   := nasm
AFLAGS               := -felf32 -g
CC                   := $(PREFIX)-gcc
INCLUDES             := -Iinclude -Iinclude/libc
CFLAGS               := -Wall -Wextra -std=gnu11 -g -O2 -ffreestanding -fstack-protector-all -nostdlib\
                        -fno-omit-frame-pointer $(INCLUDES)\
                        -DKERNEL="$(KERNEL_NAME)" -DVERSION_MAJOR="$(VERSION_MAJOR)" -DVERSION_MINOR="$(VERSION_MINOR)"\
                        -DVERSION_STR="\"$(VERSION)\"" -DARCH="\"$(ARCH)\""
LD                   := $(PREFIX)-ld
LDFLAGS              := -Ttools/kernel.ld -nostdlib -g
SOURCES              := $(shell find src/ -name "*.asm") $(shell find src/ -name "*.c")
CRTI				 := src/abi/crti.o
CRTBEGIN             := $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND				 := $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
CRTN				 := src/abi/crtn.o
OBJECTS              := $(subst .asm,.o,$(subst .c,.o,$(SOURCES)))
KERNEL               := out/isofs/boot/$(KERNEL_NAME)-kernel-$(ARCH)-$(VERSION_MAJOR).$(VERSION_MINOR)
INITRD               := out/isofs/boot/$(KERNEL_NAME)-initrd-$(ARCH)-$(VERSION_MAJOR).$(VERSION_MINOR)
MAP                  := out/initrd/boot/$(KERNEL_NAME).map
IMAGE                := out/$(KERNEL_NAME)-$(ARCH).iso
DEBUG                := out/$(KERNEL_NAME)-kernel-$(ARCH)-$(VERSION_MAJOR).$(VERSION_MINOR).debug
src/abi/stack_guard.o: CFLAGS := $(filter-out -fstack-protector-all,$(CFLAGS)) -fno-stack-protector
%.o: %.S
	@echo "\033[1;37mAssembling `basename $<`... \033[0m"
	@$(CC) $(CFLAGS) -c -o $@ $<
%.o: %.asm
	@echo "\033[1;37mAssembling `basename $<`... \033[0m"
	@$(AS) $(AFLAGS)  -o $@ $<
%.o: %.c
	@echo "\033[1;37mCompiling `basename $<`... \033[0m"
	@$(CC) $(CFLAGS) -c -o $@ $<
all: image
image: $(IMAGE)
kernel: $(KERNEL)
initrd: $(INITRD)
$(IMAGE): $(INITRD)
	@echo "\033[1;37mCreating `basename $@`... \033[0m"
	@grub-mkrescue -o $(IMAGE) out/isofs #2>/dev/null
$(KERNEL): $(CRTI) $(CRTBEGIN) $(OBJECTS) $(CRTEND) $(CRTN)
	@echo "\033[1;37mLinking `basename $@`... \033[0m"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o "$@" $^ $(shell $(CC) $(CFLAGS) --print-libgcc-file-name)
	@echo "\033[1;37mGenerating symbol table... \033[0m"
	@tools/gensymtab "$@" "$(MAP)"
	@echo "\033[1;37mCreating debug file `basename $(DEBUG)`... \033[0m"
	@cp "$@" "$(DEBUG)"
	@echo "\033[1;37mStripping `basename $@`... \033[0m"
	@strip --strip-all "$@"
$(INITRD): $(KERNEL)
	@echo "\033[1;37mGenerating initial ramdisk... \033[0m"
	@rm -f $@
	@cd out/initrd && tar -cf "../../$@" * >/dev/null
doc:
	@echo "\033[1;37mGenerating documentation... \033[0m"
	doxygen Doxyfile
run-qemu:
	@export DISPLAY=":0" ; qemu-system-i386 -cdrom "$(IMAGE)" -boot d -monitor stdio
debug-qemu:
	@export DISPLAY=":0" ; qemu-system-i386 -cdrom "$(IMAGE)" -boot d -s -S &
	@gdb -s "$(DEBUG)" -ex "target remote localhost:1234"
statistics:
	@tools/kstats
analyse:
	@cppcheck --quiet --enable=all $(INCLUDES) `find src/ -name "*.c"` `find include/ -name "*.h"` 2>&1 | grep -v "never used"
	@#clang -analyze $(INCLUDES) -Xanalyzer -analyzer-output=text `find src/ -name "*.c"` `find include/ -name "*.h"` 2>&1
xgcc:
	@cd out && ../tools/build-xgcc $(ARGS) $(TARGET)
clean:
	@echo "\033[1;37mCleaning redshift... \033[0m"
	@rm -f $(OBJECTS)
.PHONY: all image $(IMAGE) kernel $(KERNEL) initrd $(INITRD) tools doc commit run-qemu debug-qemu statistics analyse
