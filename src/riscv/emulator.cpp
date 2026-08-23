#include <src/riscv/emulator.hpp>

#include <stdexcept>


namespace riscv
{


Emulator::Emulator()
{
}

Emulator::~Emulator()
{
}

void Emulator::run(Thread& t)
{
    // TODO(Danil) instruction execution loop

    // Never return/break from the execution loop because some custom logic is executed
    // before/after every iteration. Just set shouldContinue to false.
    bool shouldContinue = true;

    for (; shouldContinue && t.getInstructionIndex() < instructions.size(); t.cycleCounter++)
    {
        Instruction instr = instructions[t.getInstructionIndex()];

        switch (instr.type)
        {
        case Instruction::Type::LUI:
            t.registers.set(instr.destinationRegister, instr.immediate);
            break;

        case Instruction::Type::AUIPC:
            t.registers.set(instr.destinationRegister, t.programCounter + instr.immediate);
            break;

        case Instruction::Type::JAL:
            t.registers.set(instr.destinationRegister, t.getNextProgramCounter());
            t.programCounter += instr.immediate;
            continue;

        case Instruction::Type::JALR:
        {
            t.registers.set(instr.destinationRegister, t.getNextProgramCounter());
            t.programCounter = t.registers[instr.sourceRegister1] + instr.immediate;
            t.programCounter &= ~0b1;
            continue;
        }

        case Instruction::Type::BEQ:
            if (t.registers[instr.sourceRegister1] == t.registers[instr.sourceRegister2])
            {
                t.programCounter += instr.immediate;
                continue;
            }
            break;

        case Instruction::Type::BNE:
            if (t.registers[instr.sourceRegister1] != t.registers[instr.sourceRegister2])
            {
                t.programCounter += instr.immediate;
                continue;
            }
            break;

        case Instruction::Type::BLT:
            if (static_cast<int64_t>(t.registers[instr.sourceRegister1])
                < static_cast<int64_t>(t.registers[instr.sourceRegister2]))
            {
                t.programCounter += instr.immediate;
                continue;
            }
            break;

        case Instruction::Type::BGE:
            if (static_cast<int64_t>(t.registers[instr.sourceRegister1])
                >= static_cast<int64_t>(t.registers[instr.sourceRegister2]))
            {
                t.programCounter += instr.immediate;
                continue;
            }
            break;

        case Instruction::Type::BLTU:
            if (t.registers[instr.sourceRegister1] < t.registers[instr.sourceRegister2])
            {
                t.programCounter += instr.immediate;
                continue;
            }
            break;

        case Instruction::Type::BGEU:
            if (t.registers[instr.sourceRegister1] >= t.registers[instr.sourceRegister2])
            {
                t.programCounter += instr.immediate;
                continue;
            }
            break;

        default:
            throw std::runtime_error("Unsupported instruction");
        }

        t.programCounter = t.getNextProgramCounter();
    }
}


} // namespace riscv
