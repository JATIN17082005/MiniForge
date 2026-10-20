#include "ir/IR.hpp"

namespace miniforge {

std::string_view irOpcodeToString(IROpcode op) {
    switch (op) {
        case IROpcode::Assign:         return "ASSIGN";
        case IROpcode::Add:            return "ADD";
        case IROpcode::Sub:            return "SUB";
        case IROpcode::Mul:            return "MUL";
        case IROpcode::Div:            return "DIV";
        case IROpcode::Mod:            return "MOD";
        case IROpcode::Neg:            return "NEG";
        case IROpcode::Equal:          return "EQ";
        case IROpcode::NotEqual:       return "NEQ";
        case IROpcode::Less:           return "LT";
        case IROpcode::LessEqual:      return "LE";
        case IROpcode::Greater:        return "GT";
        case IROpcode::GreaterEqual:   return "GE";
        case IROpcode::Label:          return "LABEL";
        case IROpcode::Jump:           return "JUMP";
        case IROpcode::JumpIfZero:     return "JUMP_IF_ZERO";
        case IROpcode::JumpIfNotZero:  return "JUMP_IF_NOT_ZERO";
        case IROpcode::Print:          return "PRINT";
    }
    return "UNKNOWN";
}

std::string IRInstruction::toString() const {
    std::ostringstream oss;
    switch (opcode) {
        case IROpcode::Assign:
            oss << dest.toString() << " = " << src1.toString();
            break;
        case IROpcode::Add:
            oss << dest.toString() << " = " << src1.toString() << " + " << src2.toString();
            break;
        case IROpcode::Sub:
            oss << dest.toString() << " = " << src1.toString() << " - " << src2.toString();
            break;
        case IROpcode::Mul:
            oss << dest.toString() << " = " << src1.toString() << " * " << src2.toString();
            break;
        case IROpcode::Div:
            oss << dest.toString() << " = " << src1.toString() << " / " << src2.toString();
            break;
        case IROpcode::Mod:
            oss << dest.toString() << " = " << src1.toString() << " % " << src2.toString();
            break;
        case IROpcode::Neg:
            oss << dest.toString() << " = -" << src1.toString();
            break;
        case IROpcode::Equal:
            oss << dest.toString() << " = (" << src1.toString() << " == " << src2.toString() << ")";
            break;
        case IROpcode::NotEqual:
            oss << dest.toString() << " = (" << src1.toString() << " != " << src2.toString() << ")";
            break;
        case IROpcode::Less:
            oss << dest.toString() << " = (" << src1.toString() << " < " << src2.toString() << ")";
            break;
        case IROpcode::LessEqual:
            oss << dest.toString() << " = (" << src1.toString() << " <= " << src2.toString() << ")";
            break;
        case IROpcode::Greater:
            oss << dest.toString() << " = (" << src1.toString() << " > " << src2.toString() << ")";
            break;
        case IROpcode::GreaterEqual:
            oss << dest.toString() << " = (" << src1.toString() << " >= " << src2.toString() << ")";
            break;
        case IROpcode::Label:
            oss << labelName << ":";
            break;
        case IROpcode::Jump:
            oss << "jump " << labelName;
            break;
        case IROpcode::JumpIfZero:
            oss << "jump_if_zero " << src1.toString() << ", " << labelName;
            break;
        case IROpcode::JumpIfNotZero:
            oss << "jump_if_not_zero " << src1.toString() << ", " << labelName;
            break;
        case IROpcode::Print:
            oss << "print " << src1.toString();
            break;
    }
    return oss.str();
}

} // namespace miniforge
