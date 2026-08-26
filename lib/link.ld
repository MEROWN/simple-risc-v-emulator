/* This is a linker script for compiling ELF executables
   for the Simple RISC-V Emulator */

/* The emulator always starts executing instructions at address 0 */
ENTRY(0)

SECTIONS {
    . = 0; /* Code address space */

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

    . = 0; /* Data address space */

    /* Small data sections */
    .sdata : {
        __global_pointer$ = . + 0x800;
        *(.sdata*)
    }
    .srodata : {
        *(.srodata*)
    }
    .sbss : {
        *(.sbss*)
    }
    /* Normal data sections */
    .rodata : {
        *(.rodata*)
    }
    .data : {
        *(.data*)
    }
    .bss : {
        *(.bss*)
        *(COMMON)
    }
    /* Arrays of function pointers that might require calling before main() */
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
}
