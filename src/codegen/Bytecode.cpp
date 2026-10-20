#include "codegen/Bytecode.hpp"

namespace miniforge {

std::string_view opcodeToString(Opcode op) {
    switch (op) {
        case Opcode::PushConst:      return "PUSH_CONST";
        case Opcode::LoadVar:        return "LOAD_VAR";
        case Opcode::StoreVar:       return "STORE_VAR";
        case Opcode::Add:            return "ADD";
        case Opcode::Sub:            return "SUB";
        case Opcode::Mul:            return "MUL";
        case Opcode::Div:            return "DIV";
        case Opcode::Mod:            return "MOD";
        case Opcode::Neg:            return "NEG";
        case Opcode::Equal:          return "EQ";
        case Opcode::NotEqual:       return "NEQ";
        case Opcode::Less:           return "LT";
        case Opcode::LessEqual:      return "LE";
        case Opcode::Greater:        return "GT";
        case Opcode::GreaterEqual:   return "GE";
        case Opcode::Jump:           return "JUMP";
        case Opcode::JumpIfZero:     return "JUMP_IF_ZERO";
        case Opcode::JumpIfNotZero:  return "JUMP_IF_NOT_ZERO";
        case Opcode::Print:          return "PRINT";
        case Opcode::Halt:           return "HALT";
    }
    return "UNKNOWN";
}

} // namespace miniforge
