export VERSION_MAJOR := 0
export VERSION_MINOR := 1
ARCH                 := i686
TARGET               := $(ARCH)-elf
PREFIX               := $(TARGET)
KERNEL               := redshift
VERSION              := $(VERSION_MAJOR).$(VERSION_MINOR)
AS                   := nasm
AFLAGS               := -felf32 -ggdb
CC                   := $(PREFIX)-gcc
CFLAGS               := -Wall -Wextra -std=gnu11 -ggdb -g3 -O0 -ffreestanding -fstack-protector-all\
                        -fno-omit-frame-pointer -Iinclude/ -Iinclude/libc\
                        -DKERNEL="$(KERNEL)" -DVERSION_MAJOR="$(VERSION_MAJOR)" -DVERSION_MINOR="$(VERSION_MINOR)"\
                        -DVERSION_STR="\"$(VERSION)\"" -DARCH="\"$(ARCH)\""
LD                   := $(PREFIX)-ld
LDFLAGS              := -Ttools/redshift.ld -nostdlib
SOURCES              := $(shell find src/ -name "*.asm") $(shell find src/ -name "*.c")
OBJECTS              := $(subst .asm,.o,$(subst .c,.o,$(SOURCES)))
REDSHIFT             := out/isofs/boot/$(KERNEL)-kernel-$(ARCH)-$(VERSION_MAJOR).$(VERSION_MINOR)
INITRD               := out/isofs/boot/$(KERNEL)-initrd-$(ARCH)-$(VERSION_MAJOR).$(VERSION_MINOR)
MAP                  := out/initrd/boot/$(KERNEL).map
IMAGE                := out/$(KERNEL)-$(ARCH).iso
DEBUG                := out/$(KERNEL)-kernel-$(ARCH)-$(VERSION_MAJOR).$(VERSION_MINOR).debug
src/abi/stack_guard.o: CFLAGS := $(filter-out -fstack-protector-all,$(CFLAGS)) -fno-stack-protector
%.o: %.asm
	@echo -e "\033[1;37mAssembling `basename $<`... \033[0m"
	@$(AS) $(AFLAGS)  -o $@ $<
%.o: %.c
	@echo -e "\033[1;37mCompiling `basename $<`... \033[0m"
	@$(CC) $(CFLAGS) -c -o $@ $<
all: image
image: $(IMAGE)
redshift: $(REDSHIFT) initrd
initrd: $(INITRD)
tools:
	@$(MAKE) -s -C tools/
$(IMAGE): redshift
	@echo -e "\033[1;37mCreating `basename $@`... \033[0m"
	@grub-mkrescue -o $(IMAGE) out/isofs 2>/dev/null
$(REDSHIFT): $(OBJECTS)
	@echo -e "\033[1;37mLinking `basename $@`... \033[0m"
	@$(CC) $(LDFLAGS) -o "$@" $^ $(shell $(CC) $(CFLAGS) --print-libgcc-file-name)
	@echo -e "\033[1;37mGenerating map file... \033[0m"
	@tools/gensymtab "$@" "$(MAP)"
	@echo -e "\033[1;37mCreating debug-only file `basename $(DEBUG)`... \033[0m"
	@cp "$@" "$(DEBUG)"
	@echo -e "\033[1;37mStripping `basename $@`... \033[0m"
	@strip --strip-all "$@"
$(INITRD): tools
	@echo -e "\033[1;37mGenerating initial ramdisk... \033[0m"
	@#tools/mkinitrd "$@" out/initrd/ >/dev/null
doc:
	@echo -e "\033[1;37mGenerating documentation... \033[0m"
	doxygen Doxyfile
run-qemu:
	@qemu-system-i386 -cdrom "$(IMAGE)" -boot d -monitor stdio
debug-qemu:
	@qemu-system-i386 -cdrom "$(IMAGE)" -boot d -s -S &
	@gdb "$(DEBUG)"
statistics:
	@tools/kstats
analyse:
	@cppcheck --quiet --enable=all `find src/ -name "*.c"` `find include/ -name "*.h"` `find src/ -name "*.s"`
clean:
	@echo -e "\033[1;37mCleaning redshift... \033[0m"
	@rm -f $(OBJECTS)
.PHONY: all image $(IMAGE) $(REDSHIFT) redshift tools doc commit run-qemu debug-qemu statistics analyse
