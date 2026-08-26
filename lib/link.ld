/* This is a linker script for compiling ELF executables
   for the Simple RISC-V Emulator */

/* The emulator always starts executing instructions at address 0 */
ENTRY(0)

SECTIONS {
    /* Code address space */
    . = 0;

    .init : {
        /* The execution starts here */
        *(.init)
        *(.init.*)
    }
    .text : {
        *(.text)
        *(.text.*)
    }
    .fini : {
        *(.fini)
        *(.fini.*)
    }

    /* Data address space */
    . = 8; /* Not 0 to avoid problems with NULL/nullptr in C/C++ */

    /* The emulator does not enforce read-only memory access, so we place
        all read-only sections at the beginning of the data address space
        in hope that it will lower the risk of accidental modification. */
    .rodata : {
        *(.rodata*)
    }
    .srodata : {
        *(.srodata*)
    }
    /* Arrays of function pointers that must be called before main() */
    .preinit_array : {
        __elf_preinit_array_start = .;
        *(.preinit_array)
        *(.preinit_array.*)
        __elf_preinit_array_end = .;
    }
    .init_array : {
        __elf_init_array_start = .;
        *(.init_array)
        *(.init_array.*)
        __elf_init_array_end = .;
    }

    .data : {
        *(.data*)
    }

    /* Small data sections that can be addressed relative to the value
        in the Global Pointer (GP) register with a small 12-bit offset
        (from -0x800 to 0x7ff) */
    .sdata : {
        /* The GP-addressable space is .sdata[0x000...0xfff] */
        __global_pointer$ = . + 0x800;

        *(.sdata*)
    }
    /* If .sdata is smaller than 4KiB, some part of .sbss can also be
        GP-addressable */
    .sbss : {
        *(.sbss*)
    }

    .bss : {
        *(.bss*)
        *(COMMON)
    }
}
