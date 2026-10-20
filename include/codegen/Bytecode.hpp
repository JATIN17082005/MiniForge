#ifndef MINIFORGE_CODEGEN_BYTECODE_HPP
#define MINIFORGE_CODEGEN_BYTECODE_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <string_view>

namespace miniforge {

enum class Opcode : uint8_t {
    PushConst,      // Pushes int64 immediate operand
    LoadVar,        // Loads variable onto stack
    StoreVar,       // Pops stack and stores to variable
    Add,            // a + b
    Sub,            // a - b
    Mul,            // a * b
    Div,            // a / b
    Mod,            // a % b
    Neg,            // -a
    Equal,          // a == b
    NotEqual,       // a != b
    Less,           // a < b
    LessEqual,      // a <= b
    Greater,        // a > b
    GreaterEqual,   // a >= b
    Jump,           // Unconditional jump to address
    JumpIfZero,     // Jump to address if top of stack is 0
    JumpIfNotZero,  // Jump to address if top of stack is != 0
    Print,          // Pops top of stack and prints it
    Halt            // Terminates execution
};

std::string_view opcodeToString(Opcode op);

struct VMInstruction {
    Opcode opcode{Opcode::Halt};
    int64_t intOperand{0};       // For PushConst, Jump target address, etc.
    std::string strOperand;      // For LoadVar, StoreVar, or symbolic label before resolution

    VMInstruction() = default;
    VMInstruction(Opcode op, int64_t intOp = 0, std::string strOp = "")
        : opcode(op), intOperand(intOp), strOperand(std::move(strOp)) {}
};

class BytecodeProgram {
public:
    BytecodeProgram() = default;

    void addInstruction(VMInstruction instr) {
        instructions_.push_back(std::move(instr));
    }

    const std::vector<VMInstruction>& instructions() const { return instructions_; }
    std::vector<VMInstruction>& instructions() { return instructions_; }

    size_t size() const { return instructions_.size(); }
    const VMInstruction& operator[](size_t index) const { return instructions_[index]; }
    VMInstruction& operator[](size_t index) { return instructions_[index]; }

private:
    std::vector<VMInstruction> instructions_;
};

} // namespace miniforge

#endif // MINIFORGE_CODEGEN_BYTECODE_HPP
