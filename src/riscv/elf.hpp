#ifndef RISCV_ELF_HPP
#define RISCV_ELF_HPP


#include <src/riscv/base.hpp>

#include <span>


namespace riscv
{

class Emulator;

/** Loads an ELF executable targeting 64-bit little-endian RISC-V into the emulator memory. */
void loadElf(std::span<uint8_t const> source, Emulator& emulator);

} // namespace riscv

#endif
