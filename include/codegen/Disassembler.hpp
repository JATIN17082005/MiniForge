#ifndef MINIFORGE_CODEGEN_DISASSEMBLER_HPP
#define MINIFORGE_CODEGEN_DISASSEMBLER_HPP

#include "codegen/Bytecode.hpp"
#include <string>

namespace miniforge {

class Disassembler {
public:
    static std::string disassemble(const BytecodeProgram& program);
    static std::string disassembleInstruction(size_t index, const VMInstruction& instr);
};

} // namespace miniforge

#endif // MINIFORGE_CODEGEN_DISASSEMBLER_HPP
