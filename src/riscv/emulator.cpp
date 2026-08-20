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

void Emulator::run()
{
    // TODO(Danil) instruction execution loop
    while (programCounter < instructions.size())
    {
        Instruction const& inst = instructions[programCounter];
        switch (inst.type)
        {
        case Instruction::Type::LUI:
            writeRegister(inst.destinationRegister, inst.immediate);
            break;
        case Instruction::Type::AUIPC:
            writeRegister(inst.destinationRegister, programCounter + inst.immediate);
            break;
        case Instruction::Type::JAL:
            writeRegister(inst.destinationRegister, programCounter);
            programCounter += inst.immediate;
            continue;
        case Instruction::Type::JALR:
        {
            Register programCounterTemp = (registers[inst.sourceRegister1] + inst.immediate) & ~1;
            writeRegister(inst.destinationRegister, programCounter);
            programCounter = programCounterTemp;
            continue;
        }
        case Instruction::Type::BEQ:
            if (registers[inst.sourceRegister1] == registers[inst.sourceRegister2])
            {
                programCounter += inst.immediate;
                continue;
            }
            break;
        case Instruction::Type::BNE:
            if (registers[inst.sourceRegister1] != registers[inst.sourceRegister2])
            {
                programCounter += inst.immediate;
                continue;
            }
            break;
        case Instruction::Type::BLT:
            if (static_cast<int64_t>(registers[inst.sourceRegister1])
                < static_cast<int64_t>(registers[inst.sourceRegister2]))
            {
                programCounter += inst.immediate;
                continue;
            }
            break;
        case Instruction::Type::BGE:
            if (static_cast<int64_t>(registers[inst.sourceRegister1])
                >= static_cast<int64_t>(registers[inst.sourceRegister2]))
            {
                programCounter += inst.immediate;
                continue;
            }
            break;
        case Instruction::Type::BLTU:
            if ((registers[inst.sourceRegister1]) < (registers[inst.sourceRegister2]))
            {
                programCounter += inst.immediate;
                continue;
            }
            break;
        case Instruction::Type::BGEU:
            if ((registers[inst.sourceRegister1]) >= (registers[inst.sourceRegister2]))
            {
                programCounter += inst.immediate;
                continue;
            }
            break;
        default:
            throw std::runtime_error("Unsupported instruction");
        }
        programCounter += 1;
    }
}


} // namespace riscv
