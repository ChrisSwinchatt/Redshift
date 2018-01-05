Things to Do:
 * [ ] Scale heap to available memory
 * [ ] Maybe convert NASM code into GNU AS
 * [ ] Maybe convert C code into C++
 * [ ] Maybe migrate to Clang
 * [ ] Stability
   * [X] Solve triple fault with TSS.
   * [x] Ensure static allocator doesn't overwrite modules, which GRUB loads immediately after the kernel.
   * [ ] Debugging facilities
     * [x] Display strings, integers and pointers.
     * [x] Dump the contents of the registers.
     * [x] Dump the contents of memory as hex values.
     * [ ] Stack trace
         * [ ] Unwind stack frames (seems to get bad values; see `src/debug/dump_stack.c`).
         * [x] Symbol lookup
             * [x] Create symbol map (see `tools/gensymtab`).
             * [x] Load symbol map
                 * [x] Load initial ramdisk
                     * [x] Define initial ramdisk (see `include/kernel/initrd.h`).
                     * [x] Create initial ramdisk.
                     * [x] Load initial ramdisk
                 * [x] Parse symbol map at `ramdisk:/boot/kernel.map`
             * [x] Map addresses to symbols.
    * [ ] Write unit tests
    * [ ] Make functions reentrant
