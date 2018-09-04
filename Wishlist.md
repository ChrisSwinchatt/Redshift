Things to Do:
 * [ ] Scale heap to available memory
 * [x] Maybe convert NASM code into GNU AS
 * x ] Maybe convert C code into C++
 * [ ] Maybe migrate to Clang
 * [ ] Sort memory map with merge sort
 * [ ] Stability
   * [X] Solve triple fault with TSS.
   * [x] Ensure static allocator doesn't overwrite modules, which GRUB loads immediately after the kernel.
   * [ ] Debugging facilities
     * [x] Display strings, integers and pointers.
     * [x] Dump the contents of the registers.
     * [x] Dump the contents of memory as hex values.
     * [ ] Stack trace
         * [x] Unwind stack frames (seems to get bad values; see `src/debug/dump_stack.c`).
         * [x] Symbol lookup
             * [x] Create symbol map (see `tools/gensymtab`).
             * [x] Load symbol map
                 * [x] Load initial ramdisk
                     * [x] Define initial ramdisk (see `include/kernel/initrd.h`).
                     * [x] Create initial ramdisk.
                     * [x] Load initial ramdisk
                 * [x] Parse symbol map at `ramdisk:/boot/kernel.map`
             * [ ] Map addresses to symbols.
    * [ ] Write unit tests
    * [x] Make functions reentrant
