This file contains specific work that needs to be done to achieve the goals outlined in the readme.

Goals:
 * [ ] Stability
   * [ ] Solve triple fault with TSS.
   * [x] Ensure static allocator doesn't overwrite modules, which GRUB loads immediately after the kernel.
   * [ ] Debugging facilities
     * [x] Display strings, integers and pointers.
     * [x] Dump the contents of the registers.
     * [x] Dump the contents of memory as hex values.
     * [ ] Stack trace
         * [-] Unwind stack frames (seems to get bad values; see src/debug/dump_stack.c).
         * [ ] Symbol lookup
             * [x] Create symbol map (see tools/gensymtab).
             * [-] Load symbol map
                 * [x] Load initial ramdisk
                     * [x] Define initial ramdisk (see include/kernel/initrd.h).
                     * [x] Create initial ramdisk (see tools/mkinitrd, tools/src/mkinitrd.cpp).
                     * [x] Load initial ramdisk
                 * [-] Parse symbol map at ramdisk:/boot/kernel.map
             * [x] Map addresses to symbols.
