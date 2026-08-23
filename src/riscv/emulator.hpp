#ifndef RISCV_EMULATOR_HPP
#define RISCV_EMULATOR_HPP


#include <src/riscv/instruction.hpp>

#include <functional>
#include <map>
#include <span>
#include <stdexcept>
#include <vector>


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


enum class BuiltinSyscall : uint64_t
{
    /** Efficiently copies memory from one region to another.

        The source and destination regions may overlap, as in `std::memmove()`.

        Arguments:
        - Argument1: the source pointer
        - Argument2: the destination pointer
        - Argument3: the number of bytes to copy */
    MemoryCopy = 0,

    /** Efficiently fills a region of memory with a given byte value.

        This is equivalent to `std::memset()`.

        Arguments:
        - Argument1: the destination pointer
        - Argument2: the byte value to fill
        - Argument3: the number of bytes to fill */
    MemoryFill = 1,

    /** Syscall numbers less than or equal to this value are reserved for built-in syscalls. */
    Max = 127,
};


class Emulator
{
public:
    Emulator();
    ~Emulator();

    void run(Thread& thread);

    /** The returned span remains valid as long as the memory is not resized. */
    std::span<uint8_t> getMemory()
    {
        return memory;
    }

    /** This may invalidate all previously acquired memory spans. */
    void resizeMemory(Size newSize)
    {
        memory.resize(newSize);
    }

    /** This moves the provided instructions into the emulator. */
    void setInstructions(std::vector<Instruction> newInstructions)
    {
        instructions = std::move(newInstructions);
    }

    /** This validates and decodes the provided program and loads it into the emulator. */
    void loadInstructions(std::span<uint8_t const> program)
    {
        setInstructions(decodeInstructions(program));
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
    std::vector<uint8_t> memory {};
    std::vector<Instruction> instructions {};
    std::map<uint64_t, CustomSyscallHandler> customSyscallHandlers {};

    void doCustomSyscall(Thread& thread, uint64_t syscallNumber)
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
