Redshift is a kernel targeting i686 and written in C, with supporting tools written in bash script and C++.

The primary goal is personal education in (and enjoyment of) kernel development, for which reason you will probably find
poor design decisions throughout. Additionally, development has been on-and-off since mid-2012, so there is a degree of
inconsistency in the design and implementation.

Design goals (see Wishlist for specifics):
 * Stable. Currently the code is in a bad shape, with several bugs and design flaws, because much of it was designed and
   implemented by a lazy 18-year-old with only a few years' experience in C. Now that the man is 20, less lazy and more
   experienced, this can be re-designed and re-implemented to be slightly better.
 * Modular. When the existing kernel code is complete and stable, the kernel will be stripped to the bare essentials
   until it is little more than a module loader, memory allocator, initial ramdisk parser and the dependencies of such.
   Currently, because the kernel is unstable, it needs a lot of debugging and other facilities built-in, but eventually
   they will be moved to modules.
 * Exokernel. When the kernel can load kernel-space modules, work can begin on userspace scheduling, memory management,
   filesystems, etc. in the form of "library operating systems" which provide optional abstraction. To support this, the
   kernel provides basic abstraction and security, e.g. representing devices as blocks rather than filesystems, and
   privileged services like memory allocation (but not management of userspace memory).
 * When version 1.0, which will be feature-complete in the above goals, is reached, the kernel can be optimised,
   bug-fixed and refactored as much as necessary. Algorithms can be changed and modules redesigned. Until then,
   development is focused on making the kernel stable enough to support all the planned features in the above goals.
   At this stage, the target may be changed to x86_64 and code may even be rewritten in C++.

Coding style:
 Lines are at most 120 columns long. Some people use an editor that wraps at 80 columns; to those people, I say "Welcome
 to the 21st century". My monitor is large enough for at least 230 characters, so be grateful I stopped at 120.

 * Indentation and braces
 `-- K&R/1TBS using 4-space indents (never tabs).
 `-- Opening braces always start on the same line as the opening statement (e.g. struct, if, ...) after a space. The
     only exceptions are functions, where the opening brace is always on the next line, and scopes that don't come after
     a compound statement.
 `-- Closing braces are always indented to the same level as the statement they close.
 `-- ALL compound statements require braces. If you see a single-line conditional or loop without braces, it's a mistake
     and you should correct it.
 `-- "else" goes on the same line as the closing brace on the previous "if".
 * Naming
 `-- function names are all lower case with underscore as a separator.
 `-- global variables must be surrounded with double underscores, e.g. "__global__".
 * Comments
 `-- Doxygen documentation comments ("doc comments") start with "/**" and end with "*/".
 `-- If a doc comment spans multiple lines, the first line ("/**") is empty and subsequent lines start with " * ",
     indented such that the asterisks align with the first asterisk in "/**" and the asterisk in "*/", which is on its
     own line. Example:
        /**
         * This is a comment
         * which documents something
         * over several lines.
         */
 `-- Normal comments start with "/*" and end with "*/". "//" single-line comments are not used.
 `-- If a normal comment spans multiple lines, the first line should contain text and subsequent lines should start with
     " * " such that the asterisks all align and "*/" is on a line by itself. Example:
        /* This is a comment
         * which spans multiple lines.
         * That is the best kind.
         */
 `-- If code is to be commented out, it is commented out with "#if 0" and "#endif".
 `-- All files must have a doc comment header containing the following filled-out tags: \file, \brief, \author (one
     per author) and \copyright. After the \copyright tag goes the license text, the "MIT/X11 license".
 `-- Excepting the file header, source files should not be documented, but (sparse) commenting in source files is
     encouraged.
 `-- 
 * Strings
 `-- Strings are in English. Strings used for output begin with a capital letter and are written with correct grammar
     and spelling without exception. The only exception for capitalisation is in panic messages because they are
     appended to the string "Kernel panic - " and therefore not starting a new sentence - don't capitalise them.
 `-- The period at the end of a line is at your discretion. As a rule of thumb, put a period at the end of a full
     sentence, but not a sentence fragment like "System memory: %d kiB" or "[0x%016X - 0x%016X] (%s)", especially if it
     contains formatted data.
 `-- Try to align data being output where it makes sense to do so, like the dump_registers function does. This is both
     for aesthetics and readability.
 `-- When calling printk, use the macro PRINTK_DEBUG for messages that might be useful for debugging but not much else,
     PRINTK_WARNING for warning messages, and PRINTK_ERROR for error messages. PRINTK_INFO is the default so you don't
     need to excplicitly write it. These macros go before the string, e.g. printk(PRINTK_DEBUG "..."). If the format
     string is not a literal, you will have to copy the macro into the buffer before the rest of its contents.
