#ifndef RISCV_EMULATOR_HPP
#define RISCV_EMULATOR_HPP


#include <src/riscv/instruction.hpp>
#include <src/riscv/utils/arena.hpp>

#include <functional>
#include <map>
#include <stdexcept>


namespace riscv
{


enum class CustomSyscallStatus
{
    /** The emulator should continue executing instructions inside `run()`. */
    Continue,
    /** The emulator should interrupt the current execution and return from `run()`. */
    Interrupt,
};

class Emulator;

using CustomSyscallHandler = std::function<CustomSyscallStatus(Thread& thread, Emulator& emulator)>;


/** Builtin syscalls. The syscall number is in `ABIRegisterIndex::Argument0`.
    Other syscall arguments are passed in `Argument1`, `Argument2`, ... */
enum class BuiltinSyscall : uint64_t
{
    /** Gets a pointer to the start of the free memory region.

        Memory regions before this pointer are allocated by the host.
        Allocating guest memory moves the free memory pointer forward, so in order for the pointer
        to stay constant, the host should not allocate any guest memory after
        the code execution starts.

        For maximum efficiency, this internally calls `memory.getFreeAlignedOffset<uint64_t>()`
        so that guest memory alignment matches the host alignment for all fundamental types.

        Returns:
        - Argument0: the free memory pointer */
    GetFreeMemory,

    /** Efficiently copies memory from one region to another.

        The regions may not overlap, as in `std::memcpy()`.
        Calling this with overlapping regions is undefined behavior.

        Arguments:
        - Argument1: the source pointer
        - Argument2: the destination pointer
        - Argument3: the number of bytes to copy */
    CopyMemory,

    /** Efficiently copies memory from one region to another.

        The regions may overlap, as in `std::memmove()`.

        Arguments:
        - Argument1: the source pointer
        - Argument2: the destination pointer
        - Argument3: the number of bytes to copy */
    CopyMemoryOverlapping,

    /** Efficiently fills a region of memory with a given byte value.

        This is equivalent to `std::memset()`.

        Arguments:
        - Argument1: the destination pointer
        - Argument2: the byte value to fill
        - Argument3: the number of bytes to fill */
    FillMemory,

    /** Compares two regions of memory.

        This is equivalent to `std::memcmp()`.

        The result is 0 if all bytes in both regions are equal.
        The result is negative if the first differing byte is less in the first region.
        The result is positive if the first differing byte is greater in the first region.

        Arguments:
        - Argument1: the first pointer
        - Argument2: the second pointer
        - Argument3: the number of bytes to compare

        Returns:
        - Argument0: the result of the comparison. */
    CompareMemory,

    /** Finds the first occurrence of a byte in a memory region.

        Arguments:
        - Argument1: the pointer to the memory region
        - Argument2: the byte to find
        - Argument3: the number of bytes to search

        Returns:
        - Argument0: the index of the first occurrence of the byte, or -1 if not found. */
    FindByte,

    /** Syscall numbers less than or equal to this value are reserved for built-in syscalls. */
    Max = 127,
};


class Emulator
{
public:
    Arena memory;

    Emulator(size_t memorySizeBytes) : memory(memorySizeBytes) { };

    ~Emulator();

    void run(Thread& thread);

    /** This moves the provided instructions into the emulator. */
    void setInstructions(std::vector<Instruction> newInstructions)
    {
        instructions = std::move(newInstructions);
    }

    /** Registers a custom syscall handler for the given syscall number.
        The syscall number must be greater than `BuiltinSyscall::Max`. */
    void registerCustomSyscallHandler(uint64_t syscallNumber, CustomSyscallHandler handler)
    {
        if (syscallNumber > static_cast<uint64_t>(BuiltinSyscall::Max))
            throw std::invalid_argument("registered syscall number must be > BuiltinSyscall::Max");

        customSyscallHandlers[syscallNumber] = std::move(handler);
    }

    void unregisterCustomSyscallHandler(uint64_t syscallNumber)
    {
        customSyscallHandlers.erase(syscallNumber);
    }


private:
    std::vector<Instruction> instructions {};
    std::map<uint64_t, CustomSyscallHandler> customSyscallHandlers {};

    /** Start time for the emulator. Used to calculate current time in all threads. */
    Clock::time_point startTime = Clock::now();


    void performCustomSyscall(Thread& thread, uint64_t syscallNumber)
    {
        auto it = customSyscallHandlers.find(syscallNumber);
        if (it != customSyscallHandlers.end())
        {
            it->second(thread, *this);
        }
    }
};


} // namespace riscv


#endif
