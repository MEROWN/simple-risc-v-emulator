# Defined by the linker itself specifically on RISC-V
    .extern __global_pointer$
# Defined by the linker script
    .extern __elf_preinit_array_start
    .extern __elf_preinit_array_end
    .extern __elf_init_array_start
    .extern __elf_init_array_end

.section .init

# Relaxing means using GP-relative addressing for small data accesses.
# The GP setup code is the only place where that is not desired.
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

# TODO runtime setup
# TODO memcpy(), memset() and other runtime functions (using syscalls)
