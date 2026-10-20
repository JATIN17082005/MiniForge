#include "codegen/Disassembler.hpp"
#include <iomanip>
#include <sstream>

namespace miniforge {

std::string Disassembler::disassembleInstruction(size_t index, const VMInstruction& instr) {
    std::ostringstream oss;
    oss << std::setfill('0') << std::right << std::setw(4) << index << "  "
        << std::setfill(' ') << std::left << std::setw(16) << opcodeToString(instr.opcode);

    switch (instr.opcode) {
        case Opcode::PushConst:
            oss << instr.intOperand;
            break;
        case Opcode::LoadVar:
        case Opcode::StoreVar:
            oss << instr.strOperand;
            break;
        case Opcode::Jump:
        case Opcode::JumpIfZero:
        case Opcode::JumpIfNotZero:
            oss << std::setfill('0') << std::right << std::setw(4) << instr.intOperand;
            if (!instr.strOperand.empty()) {
                oss << " (" << instr.strOperand << ")";
            }
            break;
        default:
            break;
    }

    return oss.str();
}

std::string Disassembler::disassemble(const BytecodeProgram& program) {
    std::ostringstream oss;
    oss << "; --- Bytecode Disassembly (" << program.size() << " instructions) ---\n";
    for (size_t i = 0; i < program.size(); ++i) {
        oss << disassembleInstruction(i, program[i]) << "\n";
    }
    return oss.str();
}

} // namespace miniforge
