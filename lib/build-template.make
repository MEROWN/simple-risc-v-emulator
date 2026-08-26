RISCV_ARGS=--target=riscv64-unknown-elf -march=rv64g -mabi=lp64d -ffreestanding
COMPILE_ARGS=$(RISCV_ARGS) -O3
LINK_ARGS=$(RISCV_ARGS) -nostdlib -T link.ld -Wl,--no-check-sections
# TODO Makefile template
