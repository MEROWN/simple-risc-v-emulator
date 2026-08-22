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
    while (t.getInstructionIndex() < instructions.size())
    {
        Instruction instr = instructions[t.getInstructionIndex()];

        switch (instr.type)
        {
        case Instruction::Type::LUI:
            t.setRegister(instr.destinationRegister, instr.immediate);
            break;

        case Instruction::Type::AUIPC:
            t.setRegister(instr.destinationRegister, t.programCounter + instr.immediate);
            break;

        case Instruction::Type::JAL:
            t.setRegister(instr.destinationRegister, t.getNextProgramCounter());
            t.programCounter += instr.immediate;
            continue;

        case Instruction::Type::JALR:
        {
            t.setRegister(instr.destinationRegister, t.getNextProgramCounter());
            t.programCounter = t.getRegister(instr.sourceRegister1) + instr.immediate;
            t.programCounter &= ~0b1;
            continue;
        }

        case Instruction::Type::BEQ:
            if (t.getRegister(instr.sourceRegister1) == t.getRegister(instr.sourceRegister2))
            {
                t.programCounter += instr.immediate;
                continue;
            }
            break;

        case Instruction::Type::BNE:
            if (t.getRegister(instr.sourceRegister1) != t.getRegister(instr.sourceRegister2))
            {
                t.programCounter += instr.immediate;
                continue;
            }
            break;

        case Instruction::Type::BLT:
            if (static_cast<int64_t>(t.getRegister(instr.sourceRegister1))
                < static_cast<int64_t>(t.getRegister(instr.sourceRegister2)))
            {
                t.programCounter += instr.immediate;
                continue;
            }
            break;

        case Instruction::Type::BGE:
            if (static_cast<int64_t>(t.getRegister(instr.sourceRegister1))
                >= static_cast<int64_t>(t.getRegister(instr.sourceRegister2)))
            {
                t.programCounter += instr.immediate;
                continue;
            }
            break;

        case Instruction::Type::BLTU:
            if (t.getRegister(instr.sourceRegister1) < t.getRegister(instr.sourceRegister2))
            {
                t.programCounter += instr.immediate;
                continue;
            }
            break;

        case Instruction::Type::BGEU:
            if (t.getRegister(instr.sourceRegister1) >= t.getRegister(instr.sourceRegister2))
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
